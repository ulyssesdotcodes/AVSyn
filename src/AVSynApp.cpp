#include "boost\range\adaptor\map.hpp"

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
#include "cinder/Display.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

#include "AudioShaderVisualization.h"
#include "ChoiceVisualization.h"
#include "DotsVisualization.h"
#include "EQPointCloud.h"
#include "Feedback.h"
#include "FlockingVisualization.h"
#include "Fluid.h"
#include "Lights.h"
#include "Mix.h"
#include "ShaderVisualization.h"
#include "Video.h"
#include "Visualization.h"

#include "DeltaSource.h"

// Unused for now
//#include "TreeVisualization.h"
//#include "KinectParticles.h"

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

	ChoiceVisualization mMainVisualization;
};

void AVSynApp::setup()
{
	// Set up world
	mWorld.camera = unique_ptr<CameraPersp>(new CameraPersp(getWindowWidth(), getWindowHeight(), 50));
	mWorld.camera->setPerspective(60.0f, getWindowAspectRatio(), 1.0f, 3000.0f);
	mWorld.camera->lookAt(vec3(0.0, 0.0, 100.0f), vec3(0));
	mWorld.audioSource = unique_ptr<AudioSource>(new AudioSource());
	mWorld.deltaSource = unique_ptr<DeltaSource>(new DeltaSource());
	mWorld.beatDetector = unique_ptr<BeatDetector>(new BeatDetector());
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
	map<string, shared_ptr<Visualization>> visualizations;

	auto simpleVis = make_shared<AudioShaderVisualization>();
	simpleVis->setup("simple.frag");
	visualizations.insert(make_pair("Simple", simpleVis));

	auto circularVis = make_shared<AudioShaderVisualization>();
	circularVis->setup("circular_fft.frag");
	visualizations.insert(make_pair("Circular", circularVis));

	auto flocking = make_shared<FlockingVisualization>();
	flocking->setup();
	visualizations.insert(make_pair("Flocking", flocking));

	auto dotsVis = make_shared<DotsVisualization>();
	dotsVis->setup();
	visualizations.insert(make_pair("Dots", dotsVis));

	auto eqPointCloud = make_shared<EQPointCloud>();
	eqPointCloud->setup();
	visualizations.insert(make_pair("EQPointCloud", eqPointCloud));

	//auto *trees = new TreeVisualization();
	//trees->setup(mAudioSource, mBeatDetector);
	//mVisualizations.insert(make_pair("Trees", trees));
	//mVisualizationOptions.push_back("Trees");

	auto fluid = make_shared<Fluid>();
	fluid->setup();
	visualizations.insert(make_pair("Fluid", fluid));

	//auto *kickChangeImage = new KinectParticles();
	//kickChangeImage->setup(mAudioSource, mBeatDetector, mVisualizations, mVisualizationOptions);
	//mVisualizations.insert(make_pair("Kick Change Image", kickChangeImage));
	//mVisualizationOptions.push_back("Kick Change Image");

	auto bufferVis = make_shared<Feedback>();
	bufferVis->setup("Feedback/buffer.frag");
	visualizations.insert(make_pair("Buffer", bufferVis));

	auto rotateVis = make_shared<Feedback>();
	rotateVis->setup("Feedback/rotate.frag");
	visualizations.insert(make_pair("Rotate", rotateVis));

	auto lightsVis = make_shared<Lights>();
	lightsVis->setup();
	visualizations.insert(make_pair("Lights", lightsVis));

	auto wheatFields = make_shared<Video>();
	wheatFields->setup("Wheat Field.mov");
	visualizations.insert(make_pair("Wheat Fields", wheatFields));

	auto mix = make_shared<Mix>();
	mix->setup(visualizations);
	visualizations.insert(make_pair("Mix", mix));

	mMainVisualization = ChoiceVisualization();
	mMainVisualization.setup(visualizations);
	mMainVisualization.switchParams(mParams, "Main");
	
	// Setup rendering!
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
	mMainVisualization.update(mWorld);
}

void AVSynApp::drawRender()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	mMainVisualization.draw(mWorld);
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
