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
//#include "TreeVisualization.h"
//#include "KinectParticles.h"
#include "Fluid.h"
#include "Mix.h"
#include "Feedback.h"
#include "Video.h"
#include "Lights.h"

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

private:
	World mWorld;

	WindowRef mParamWindow;
	params::InterfaceGlRef mParams;
	quat mSceneRotation;
	bool mSaveFrames;

	Visualization *mVisualization;

	map<string, Visualization*> mVisualizations;
	vector<string> mVisualizationOptions;
	int mCurrentVisOption;
};

void AVSynApp::setup()
{
	vector<DisplayRef> displays = Display::getDisplays();

	
	mWorld.camera = unique_ptr<CameraPersp>(new CameraPersp(getWindowWidth(), getWindowHeight(), 50));
	mWorld.camera->setPerspective(60.0f, getWindowAspectRatio(), 1.0f, 3000.0f);
	mWorld.camera->lookAt(vec3(0.0, 0.0, 100.0f), vec3(0));

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

	mWorld.audioSource = unique_ptr<AudioSource>(new AudioSource());
	mWorld.deltaSource = unique_ptr<DeltaSource>(new DeltaSource());
	mWorld.beatDetector = unique_ptr<BeatDetector>(new BeatDetector());
	mWorld.audioSource->setup();
	mWorld.windowSize = getWindowIndex(0)->getSize();
	mWorld.bounds = getWindowIndex(0)->getBounds();

	AudioShaderVisualization *simpleVis = new AudioShaderVisualization();
	simpleVis->setup("simple.frag");
	mVisualizations.insert(make_pair("Simple", simpleVis));
	mVisualizationOptions.push_back("Simple");

	AudioShaderVisualization *circularVis = new AudioShaderVisualization();
	circularVis->setup("circular_fft.frag");
	mVisualizations.insert(make_pair("Circular", circularVis));
	mVisualizationOptions.push_back("Circular");

	auto *flocking = new FlockingVisualization();
	flocking->setup();
	mVisualizations.insert(make_pair("Flocking", flocking));
	mVisualizationOptions.push_back("Flocking");

	auto *dotsVis = new DotsVisualization();
	dotsVis->setup();
	mVisualizations.insert(make_pair("Dots", dotsVis));
	mVisualizationOptions.push_back("Dots");

	auto *eqPointCloud = new EQPointCloud();
	eqPointCloud->setup();
	mVisualizations.insert(make_pair("EQPointCloud", eqPointCloud));
	mVisualizationOptions.push_back("EQPointCloud");

	//auto *trees = new TreeVisualization();
	//trees->setup(mAudioSource, mBeatDetector);
	//mVisualizations.insert(make_pair("Trees", trees));
	//mVisualizationOptions.push_back("Trees");

	auto *fluid = new Fluid();
	fluid->setup();
	mVisualizations.insert(make_pair("Fluid", fluid));
	mVisualizationOptions.push_back("Fluid");

	//auto *kickChangeImage = new KinectParticles();
	//kickChangeImage->setup(mAudioSource, mBeatDetector, mVisualizations, mVisualizationOptions);
	//mVisualizations.insert(make_pair("Kick Change Image", kickChangeImage));
	//mVisualizationOptions.push_back("Kick Change Image");

	auto *bufferVis = new Feedback();
	bufferVis->setup("Feedback/buffer.frag");
	mVisualizations.insert(make_pair("Buffer", bufferVis));
	mVisualizationOptions.push_back("Buffer");

	auto *rotateVis = new Feedback();
	rotateVis->setup("Feedback/rotate.frag");
	mVisualizations.insert(make_pair("Rotate", rotateVis));
	mVisualizationOptions.push_back("Rotate");

	auto *lightsVis = new Lights();
	lightsVis->setup();
	mVisualizations.insert(make_pair("Lights", lightsVis));
	mVisualizationOptions.push_back("Lights");

	auto *wheatFields = new Video();
	wheatFields->setup("Wheat Field.mov");
	mVisualizations.insert(make_pair("Wheat Fields", wheatFields));
	mVisualizationOptions.push_back("Wheat Fields");

	auto *mix = new Mix();
	mix->setup(mVisualizations, mVisualizationOptions);
	mVisualizations.insert(make_pair("Mix", mix));
	mVisualizationOptions.push_back("Mix");

	mCurrentVisOption = mVisualizations.size() - 1;
	mVisualization = mVisualizations[mVisualizationOptions[mCurrentVisOption]];
	
	mVisualization->switchParams(mParams, "Main");

	mParams->addParam("Visualizations", mVisualizationOptions, 
		[=](int ind) {
			mVisualization->resetParams(mParams);
			mCurrentVisOption = ind;
			mVisualization = mVisualizations[mVisualizationOptions[mCurrentVisOption]];
			mVisualization->switchParams(mParams, "Main");
		},
		[=]() {
			return mCurrentVisOption;
		}
		);

	getWindowIndex(0)->getSignalDraw().connect([=]() { drawRender(); });
}

void AVSynApp::keyDown(KeyEvent event) {
	if (event.getChar() == 'q') {
		quit();
	}
}

void AVSynApp::update()
{
	getWindowIndex(0)->getRenderer()->makeCurrentContext();
	mWorld.deltaSource->update();
	mVisualization->update(mWorld);
}

void AVSynApp::drawRender()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	mVisualization->draw(mWorld);

	if (mSaveFrames) {
		writeImage(app::getAppPath().generic_string() + "\\save scene\\image_" + to_string(app::getElapsedFrames()) + ".png", copyWindowSurface());
	}
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
