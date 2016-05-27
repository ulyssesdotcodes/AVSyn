#include "boost\range\adaptor\map.hpp"

#include "cinder/app/App.h"
#include "cinder/GeomIo.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/Display.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "AudioShaderVisualization.h"
#include "BlankVisualization.h"
#include "ChoiceVisualization.h"
#include "DotsVisualization.h"
#include "EQPointCloud.h"
#include "Feedback.h"
#include "FlockingVisualization.h"
#include "FluidVisualizer.h"
#include "Lights.h"
#include "Mix.h"
#include "NeuronVisualization.h"
#include "OscController.h"
#include "OscVisController.h"
#include "ParticleVisualization.h"
#include "ShaderVisualization.h"
#include "Video.h"
#include "Visualization.h"

#include "PingPongFBO.h"

#include "DeltaSource.h"

// Unused for now
//#include "TreeVisualization.h"
//#include "KinectParticles.h"

using namespace ci;
using namespace ci::app;

class AVSynApp : public App {
public:
	void setup() override;
	void update() override;
	void keyDown(KeyEvent event) override;
	void drawRender();
	void drawParams() ;

	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;

private:
	World mWorld;

	WindowRef mParamWindow;
	params::InterfaceGlRef mParams;

	std::unique_ptr<Mix> mMainVisualization;


	//////////////////////
	// Projection mapping variables

	bool mRenderVertices;
	std::vector<vec3> mVertices;
	vec3* mSelectedVertex;
	std::vector<vec3> mPlaneVertices;
	int mWidth;

	gl::VboRef mVbo;
	gl::VboMeshRef mVboMesh;

	gl::BatchRef mDrawVertex;
	gl::BatchRef mDrawRect;

	gl::FboRef mFbo;

	std::shared_ptr<OscController> mOscController;

	Font mFont;
};

void AVSynApp::setup()
{
	mFont = Font("Quicksand Book Regular", 12.0f);

	// Set up world
	mWorld.camera = std::unique_ptr<CameraPersp>(new CameraPersp(getWindowWidth(), getWindowHeight(), 50));
	mWorld.camera->setPerspective(60.0f, getWindowAspectRatio(), 1.0f, 3000.0f);
	mWorld.camera->lookAt(vec3(0.0, 0.0, 100.0f), vec3(0));
	mWorld.audioSource = std::unique_ptr<AudioSource>(new AudioSource());
	mWorld.deltaSource = std::unique_ptr<DeltaSource>(new DeltaSource());
	mWorld.beatDetector = std::unique_ptr<BeatDetector>(new BeatDetector());
	mWorld.audioSource->setup();
	mWorld.windowSize = getWindowIndex(0)->getSize();
	mWorld.bounds = getWindowIndex(0)->getBounds();

	// Set up params
	mOscController = std::make_shared<OscController>();

	//vec2 paramsSize = vec2(255, 512);
	//auto format = Window::Format();
	//format.setSize(paramsSize + vec2(40, 40));
	//format.setPos(ivec2(100, 100));
	//mParamWindow = createWindow(format);
	//mParamWindow->getSignalDraw().connect([=]() { drawParams(); });
	//mParams = params::InterfaceGl::create(getWindow(), "Options", paramsSize);

	//mParams->addParam("Render Vertices", &mRenderVertices);

	//// Reset to the regular window instead of params
	//getWindowIndex(0)->getRenderer()->makeCurrentContext();

	// Create visualizations list
	std::map<std::string, std::shared_ptr<Visualization>> visualizations;

	auto blank = std::make_shared<BlankVisualization>();
	visualizations.insert(make_pair("Blank", blank));

	auto simpleVis = std::make_shared<AudioShaderVisualization>("circle.frag");
	visualizations.insert(make_pair("Simple", simpleVis));

	auto circularVis = std::make_shared<AudioShaderVisualization>("circular_fft.frag");
	visualizations.insert(make_pair("Circular", circularVis));

	auto flocking = std::make_shared<FlockingVisualization>();
	visualizations.insert(make_pair("Flocking", flocking));

	auto dotsVis = std::make_shared<DotsVisualization>();
	visualizations.insert(make_pair("Dots", dotsVis));

	auto eqPointCloud = std::make_shared<EQPointCloud>();
	visualizations.insert(make_pair("EQPointCloud", eqPointCloud));

	//auto *trees = new TreeVisualization();
	//trees->setup(mAudioSource, mBeatDetector);
	//mVisualizations.insert(make_pair("Trees", trees));
	//mVisualizationOptions.push_back("Trees");

	auto fluid = std::make_shared<FluidVisualizer>();
	visualizations.insert(make_pair("Smoke", fluid));

	//auto *kickChangeImage = new KinectParticles();
	//kickChangeImage->setup(mAudioSource, mBeatDetector, mVisualizations, mVisualizationOptions);
	//mVisualizations.insert(make_pair("Kick Change Image", kickChangeImage));
	//mVisualizationOptions.push_back("Kick Change Image");

	auto bufferVis = std::make_shared<Feedback>("Feedback/buffer.frag");
	visualizations.insert(make_pair("Buffer", bufferVis));

	auto rotateVis = std::make_shared<Feedback>("Feedback/rotate.frag");
	visualizations.insert(make_pair("Rotate", rotateVis));

	auto lightsVis = std::make_shared<Lights>();
	visualizations.insert(make_pair("Lights", lightsVis));

	auto neuronsVis = std::make_shared<NeuronVisualization>(mWorld);
	visualizations.insert(make_pair("Neurons", neuronsVis));

	//auto videoVis = std::make_shared<Video>("C:\\Users\\Ulysses\\Development\\AVSyn\\assets\\Purity Ring - Fireshrine.mp4");
	//visualizations.insert(make_pair("Video", videoVis));

	auto particlesVis = std::make_shared<ParticleVisualization>();
	visualizations.insert(make_pair("Particles", particlesVis));

	mMainVisualization = 
		std::make_unique<Mix>(mWorld, visualizations, mOscController);

	// Setup rendering!
	vec2 resolution = getWindowIndex(0)->getSize();

	std::vector<vec2> texCoordsVertices;
	mVertices.push_back(vec3(0, 0, 0));
	texCoordsVertices.push_back(vec2(0, 1));
	mVertices.push_back(vec3(resolution.x, 0, 0));
	texCoordsVertices.push_back(vec2(1, 1));
	mVertices.push_back(vec3(0, resolution.y, 0));
	texCoordsVertices.push_back(vec2(0, 0));
	mVertices.push_back(vec3(resolution.x, resolution.y, 0));
	texCoordsVertices.push_back(vec2(1, 0));

	mDrawVertex = gl::Batch::create(geom::Circle().radius(16), gl::getStockShader(gl::ShaderDef().color()));

	//geom::BufferLayout layout = geom::BufferLayout();
	//layout.append(geom::POSITION, 3, sizeof(vec3), 0);

	//mVbo = gl::Vbo::create(GL_ARRAY_BUFFER, mVertices.size() * sizeof(vec3), mVertices.data(), GL_DYNAMIC_DRAW);

	//geom::BufferLayout texCoordsLayout = geom::BufferLayout();
	//texCoordsLayout.append(geom::TEX_COORD_0, 2, sizeof(vec2), 0);

	mRenderVertices = false;
	mWidth = 32;

	auto plane = 
		geom::Plane()
			.size(vec2(resolution.x * 0.5, resolution.y * 0.5))
			.subdivisions(ivec2(mWidth, mWidth))
			.normal(vec3(0,0,1))
			.origin(vec3(resolution.x * 0.25, resolution.y * 0.25, 0));

	std::vector<gl::VboMesh::Layout> bufferLayout = {
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::POSITION, 3),
		gl::VboMesh::Layout().usage(GL_STATIC_DRAW).attrib(geom::Attrib::TEX_COORD_0, 2)
	};

	mVboMesh = gl::VboMesh::create(plane, bufferLayout);

	auto mappedPosAttrib = mVboMesh->mapAttrib3f( geom::Attrib::POSITION, false );
	for( int i = 0; i < mVboMesh->getNumVertices(); i++ ) {
		vec3 &pos = *mappedPosAttrib;
		mPlaneVertices.push_back(vec3(pos));
		++mappedPosAttrib;
	}
	mappedPosAttrib.unmap();

	mDrawRect = gl::Batch::create(mVboMesh, gl::getStockShader(gl::ShaderDef().texture()));

	mFbo = gl::Fbo::create(resolution.x, resolution.y);

	getWindowIndex(0)->getSignalDraw().connect([=]() { drawRender(); });
}

void AVSynApp::keyDown(KeyEvent event) {
	if (event.getChar() == 'q') {
		quit();
	}
}

void AVSynApp::mouseDown( MouseEvent event )
{
	float dist = glm::length(vec3(event.getPos(), 0) - mVertices[0]);
	float distIndex = 0;

	// Calculate the closest point
	for (int i = 1; i < mVertices.size(); ++i) {
		float newDist = glm::length(vec3(event.getPos(), 0) - mVertices[i]);
		if (newDist < dist) {
			dist = newDist;
			distIndex = i;
		}
	}

	mSelectedVertex = &mVertices[distIndex];
}

void AVSynApp::mouseDrag(MouseEvent event)
{
	*mSelectedVertex = vec3(event.getPos(), 0);
}

void AVSynApp::update()
{
	getWindowIndex(0)->getRenderer()->makeCurrentContext();
	mWorld.deltaSource->update();
	mWorld.beatDetector->update(mWorld, 1.2);
	mMainVisualization->update(mWorld);

	float offset = getElapsedSeconds() * 4.0f;

	for (int i = 0; i < mPlaneVertices.size(); i++) {
		int x = i / (mWidth + 1);
		int y = mWidth - i % (mWidth + 1);

		float px = (float)x / mWidth;
		float py = (float)y / mWidth;

		float startX = glm::mix(mVertices[0].x, mVertices[2].x, py);
		float endX = glm::mix(mVertices[1].x, mVertices[3].x, py);
		float newX = glm::mix(startX, endX, px);

		float startY = glm::mix(mVertices[0].y, mVertices[1].y, px);
		float endY = glm::mix(mVertices[2].y, mVertices[3].y, px);
		float newY = glm::mix(startY, endY, py);


		vec3* pos = &mPlaneVertices[i];
		pos->x = newX;
		pos->y = newY;

		//app::console() << "x: " << pos->x << " y: " << pos->y << endl;
		//app::console() << "x: " << x << " y: " << y << endl;
		//app::console() << "x: " << newX << " y: " << newY << endl;
	}

	mVboMesh->bufferAttrib(geom::POSITION, mPlaneVertices);
}

void AVSynApp::drawRender()
{
	//mVbo->bufferSubData(0, mVertices.size() * sizeof(vec3), mVertices.data());

	//getWindowIndex(0)->getRenderer()->makeCurrentContext();

	{
		gl::ScopedFramebuffer scpfbo(mFbo);

		gl::clear( Color( 0, 0, 0 ) ); 

		mMainVisualization->draw(mWorld);
	}

	gl::clear( Color( 0, 0, 0 ) ); 

	// Draw the rectangle
	gl::ScopedViewport vp(ivec2(0), mFbo->getSize());
	gl::ScopedTextureBind texScope(mFbo->getColorTexture());
	gl::pushMatrices();
	gl::setMatricesWindow(mFbo->getSize());
	mDrawRect->draw();

	// Draw the vertices
	if(mRenderVertices) {
		gl::ScopedColor color(1, 0, 0);

		for (int i = 0; i < mVertices.size(); ++i) {
			gl::ScopedModelMatrix scpModelMatrix;
			gl::translate(mVertices[i]);
			mDrawVertex->draw();
		}

		gl::popMatrices();
	}

	//gl::drawString("Framerate: " + std::to_string(getAverageFps()), vec2(10));
}

void AVSynApp::drawParams()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	mParams->draw();
}

CINDER_APP(AVSynApp, RendererGl(), [&](App::Settings *settings) {
	FullScreenOptions options;
	std::vector<DisplayRef> displays = Display::getDisplays();
	settings->setConsoleWindowEnabled();
	settings->setFullScreen(true, options);	
	if (displays.size() > 1) {
		settings->setDisplay(displays[1]);
	}
	settings->setFrameRate(60.0f);
})
