#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder\params\Params.h"
#include "cinder\Camera.h"
#include "cinder\Display.h"
#include "boost\range\adaptor\map.hpp"

#include "Visualization.h"
#include "AudioShaderVisualization.h"
#include "ShaderVisualization.h"
#include "DotsVisualization.h"
#include "EQPointCloud.h"
#include "FlockingVisualization.h"
#include "TreeVisualization.h"
#include "KinectParticles.h"
#include "Fluid.h"
#include "Mix.h"
#include "Feedback.h"

#include "PingPongFBO.h"

#include "DeltaSource.h"

using namespace ci;
using namespace ci::app;
using namespace std;

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
	WindowRef mParamWindow;
	params::InterfaceGlRef mParams;
	quat mSceneRotation;
	bool mSaveFrames;

	CameraPersp mCam;
	vec3 mEye;
	vec3 mCenter;
	vec3 mUp;

	Visualization *mVisualization;
	AudioSource* mAudioSource;
	DeltaSource* mDeltaSource;
	BeatDetector* mBeatDetector;

	map<string, Visualization*> mVisualizations;
	vector<string> mVisualizationOptions;
	int mCurrentVisOption;

	vector<vec3> mVertices;
	vec3* mSelectedVertex;

	gl::VboRef mVbo;
	gl::VboMeshRef mVboMesh;

	gl::BatchRef mDrawVertex;
	gl::BatchRef mDrawRect;

	gl::FboRef mFbo;
};

void AVSynApp::setup()
{
	vector<DisplayRef> displays = Display::getDisplays();

	mCam = CameraPersp(getWindowWidth(), getWindowHeight(), 50);
	mCam.setPerspective(60.0f, getWindowAspectRatio(), 1.0f, 3000.0f);
	mEye = vec3(0.0, 0.0, 100.0f);
	mCenter = vec3(0);
	mCam.lookAt(mEye, mCenter);

	vec2 paramsSize = vec2(255, 512);
	mCurrentVisOption = 0;
	auto format = Window::Format();
	format.setSize(paramsSize + vec2(40, 40));
	format.setPos(ivec2(100, 100));
	mParamWindow = createWindow(format);
	mParamWindow->getSignalDraw().connect([=]() { drawParams(); });
	mParams = params::InterfaceGl::create(getWindow(), "Options", paramsSize);

	mParams->addParam("Rotation", &mSceneRotation);
	mSaveFrames = false;
	mParams->addParam("Record", &mSaveFrames);

	getWindowIndex(0)->getRenderer()->makeCurrentContext();

	mAudioSource = new AudioSource();
	mDeltaSource = new DeltaSource();
	mBeatDetector = new BeatDetector(mAudioSource);
	mAudioSource->setup();

	AudioShaderVisualization *simpleVis = new AudioShaderVisualization();
	simpleVis->setup(mAudioSource, "simple.frag");
	mVisualizations.insert(make_pair("Simple", simpleVis));
	mVisualizationOptions.push_back("Simple");

	AudioShaderVisualization *circularVis = new AudioShaderVisualization();
	circularVis->setup(mAudioSource, "circular_fft.frag");
	mVisualizations.insert(make_pair("Circular", circularVis));
	mVisualizationOptions.push_back("Circular");

	auto *flocking = new FlockingVisualization();
	flocking->setup(mAudioSource, mDeltaSource, mBeatDetector);
	mVisualizations.insert(make_pair("Flocking", flocking));
	mVisualizationOptions.push_back("Flocking");

	auto *dotsVis = new DotsVisualization();
	dotsVis->setup(mAudioSource, mBeatDetector);
	mVisualizations.insert(make_pair("Dots", dotsVis));
	mVisualizationOptions.push_back("Dots");

	auto *eqPointCloud = new EQPointCloud();
	eqPointCloud->setup(mAudioSource);
	mVisualizations.insert(make_pair("EQPointCloud", eqPointCloud));
	mVisualizationOptions.push_back("EQPointCloud");

	//auto *trees = new TreeVisualization();
	//trees->setup(mAudioSource, mBeatDetector);
	//mVisualizations.insert(make_pair("Trees", trees));
	//mVisualizationOptions.push_back("Trees");

	auto *fluid = new Fluid();
	fluid->setup(mAudioSource, mBeatDetector);
	mVisualizations.insert(make_pair("Fluid", fluid));
	mVisualizationOptions.push_back("Fluid");

	auto *kickChangeImage = new KinectParticles();
	kickChangeImage->setup(mAudioSource, mBeatDetector, mVisualizations, mVisualizationOptions);
	mVisualizations.insert(make_pair("Kick Change Image", kickChangeImage));
	mVisualizationOptions.push_back("Kick Change Image");

	auto *bufferVis = new Feedback();
	bufferVis->setup(mAudioSource, "Feedback/buffer.frag");
	mVisualizations.insert(make_pair("Buffer", bufferVis));
	mVisualizationOptions.push_back("Buffer");

	auto *rotateVis = new Feedback();
	rotateVis->setup(mAudioSource, "Feedback/rotate.frag");
	mVisualizations.insert(make_pair("Rotate", rotateVis));
	mVisualizationOptions.push_back("Rotate");

	auto *mix = new Mix();
	mix->setup(mVisualizations, mVisualizationOptions);
	mVisualizations.insert(make_pair("Mix", mix));
	mVisualizationOptions.push_back("Mix");

	mCurrentVisOption = mVisualizations.size() - 1;
	mVisualization = mVisualizations[mVisualizationOptions[mCurrentVisOption]];
	
	mVisualization->switchCamera(&mCam);
	mVisualization->switchParams(mParams, "Main");

	mParams->addParam("Visualizations", mVisualizationOptions, 
		[=](int ind) {
			mVisualization->resetParams(mParams);
			mCurrentVisOption = ind;
			mVisualization = mVisualizations[mVisualizationOptions[mCurrentVisOption]];
			mVisualization->switchCamera(&mCam);
			mVisualization->switchParams(mParams, "Main");
		},
		[=]() {
			return mCurrentVisOption;
		}
		);

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

	vector<vec2> texCoordsVertices;
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
	mDeltaSource->update();
	mVisualization->update();

	gl::ScopedFramebuffer scpfbo(mFbo);

	gl::clear( Color( 0, 0, 0 ) ); 
	gl::ScopedViewport vp(ivec2(0), mFbo->getSize());
	gl::pushMatrices();
	if (mVisualization->perspective()) {
		gl::setMatrices(mCam);
		gl::rotate(mSceneRotation);
		gl::enableDepthRead();
		gl::enableDepthWrite();
	}
	else {
		gl::setMatricesWindow(mFbo->getSize());
	}
	mVisualization->draw();
	gl::popMatrices();
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
	vector<DisplayRef> displays = Display::getDisplays();
	settings->setFullScreen(true, options);	
	if (displays.size() > 1) {
		settings->setDisplay(displays[1]);
	}
	settings->setFrameRate(60.0f);
})
