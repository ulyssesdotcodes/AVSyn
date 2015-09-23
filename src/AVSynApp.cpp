#include "boost\range\adaptor\map.hpp"

#include "cinder/app/App.h"
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

	std::unique_ptr<ChoiceVisualization> mMainVisualization;

	std::vector<vec3> mVertices;
	vec3* mSelectedVertex;

	gl::VboRef mVbo;
	gl::VboMeshRef mVboMesh;

	gl::BatchRef mDrawVertex;
	gl::BatchRef mDrawRect;

	gl::FboRef mFbo;
};

void AVSynApp::setup()
{
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
	vec2 paramsSize = vec2(255, 512);
	auto format = Window::Format();
	format.setSize(paramsSize + vec2(40, 40));
	format.setPos(ivec2(100, 100));
	mParamWindow = createWindow(format);
	mParamWindow->getSignalDraw().connect([=]() { drawParams(); });
	mParams = params::InterfaceGl::create(getWindow(), "Options", paramsSize);


	// Reset to the regular window instead of params
	getWindowIndex(0)->getRenderer()->makeCurrentContext();

	// Create visualizations list
	std::map<std::string, std::shared_ptr<Visualization>> visualizations;

	auto blank = std::make_shared<BlankVisualization>();
	visualizations.insert(make_pair("Blank", blank));

	auto simpleVis = std::make_shared<AudioShaderVisualization>("simple.frag");
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
	visualizations.insert(make_pair("Fluid", fluid));

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

	auto mix = std::make_shared<Mix>(visualizations);
	visualizations.insert(make_pair("Mix", mix));

	mMainVisualization = std::make_unique<ChoiceVisualization>(mWorld, visualizations);
	mMainVisualization->switchParams(mParams, "Main");

	// Setup rendering!
	vec2 resolution = getWindowIndex(0)->getSize();
	mVertices.push_back(vec3(0, 0, 0));
	mVertices.push_back(vec3(0, resolution.y * 0.5, 0));
	mVertices.push_back(vec3(resolution.x * 0.5, resolution.y * 0.5, 0));
	mVertices.push_back(vec3(resolution.x * 0.5, 0, 0));

	mDrawVertex = gl::Batch::create(geom::Circle().radius(16), gl::getStockShader(gl::ShaderDef().color()));

	geom::BufferLayout layout = geom::BufferLayout();
	layout.append(geom::POSITION, 3, sizeof(vec3), 0);

	mVbo = gl::Vbo::create(GL_ARRAY_BUFFER, mVertices.size() * sizeof(vec3), mVertices.data(), GL_DYNAMIC_DRAW);

	geom::BufferLayout texCoordsLayout = geom::BufferLayout();
	texCoordsLayout.append(geom::TEX_COORD_0, 2, sizeof(vec2), 0);

	std::vector<vec2> texCoordsVertices;
	texCoordsVertices.push_back(vec2(0, 1));
	texCoordsVertices.push_back(vec2(0, 0));
	texCoordsVertices.push_back(vec2(1, 0));
	texCoordsVertices.push_back(vec2(1, 1));

	gl::VboRef texCoords = gl::Vbo::create(GL_ARRAY_BUFFER, texCoordsVertices.size() * sizeof(vec2), texCoordsVertices.data());

	mVboMesh = gl::VboMesh::create(mVertices.size(), GL_TRIANGLE_FAN, { { layout, mVbo }, { texCoordsLayout, texCoords } });

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
	mMainVisualization->update(mWorld);

	gl::ScopedFramebuffer scpfbo(mFbo);

	gl::clear( Color( 0, 0, 0 ) ); 

	mMainVisualization->draw(mWorld);
}

void AVSynApp::drawRender()
{
	mVbo->bufferSubData(0, mVertices.size() * sizeof(vec3), mVertices.data());

	getWindowIndex(0)->getRenderer()->makeCurrentContext();
	gl::clear( Color( 0, 0, 0 ) ); 

	// Draw the rectangle
	gl::ScopedViewport vp(ivec2(0), mFbo->getSize());
	gl::ScopedTextureBind texScope(mFbo->getColorTexture());
	gl::pushMatrices();
	gl::setMatricesWindow(mFbo->getSize());
	mDrawRect->draw();

	// Draw the vertices
	gl::ScopedColor color(1, 0, 0);

	for (int i = 0; i < mVertices.size(); ++i) {
		gl::ScopedModelMatrix scpModelMatrix;
		gl::translate(mVertices[i]);
		mDrawVertex->draw();
	}

	gl::popMatrices();
}

void AVSynApp::drawParams()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	mParams->draw();
}

CINDER_APP(AVSynApp, RendererGl(), [&](App::Settings *settings) {
	FullScreenOptions options;
	std::vector<DisplayRef> displays = Display::getDisplays();
	settings->setFullScreen(true, options);	
	if (displays.size() > 1) {
		settings->setDisplay(displays[1]);
	}
	settings->setFrameRate(60.0f);
})
