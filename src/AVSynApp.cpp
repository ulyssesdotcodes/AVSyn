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
#include "ExplosionsVisualizer.h"
#include "Feedback.h"
#include "FlockingVisualization.h"
#include "FluidVisualizer.h"
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

	std::unique_ptr<ChoiceVisualization> mMainVisualization;
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

	auto explosions = std::make_shared<ExplosionsVisualizer>();
	visualizations.insert(make_pair("Explosions", explosions));

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

	auto wheatFields = std::make_shared<Video>("Wheat Field.mov");
	visualizations.insert(make_pair("Wheat Fields", wheatFields));

	auto mix = std::make_shared<Mix>(visualizations);
	visualizations.insert(make_pair("Mix", mix));

	mMainVisualization = std::make_unique<ChoiceVisualization>(mWorld, visualizations);
	mMainVisualization->switchParams(mParams, "Main");

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
	mMainVisualization->update(mWorld);
}

void AVSynApp::drawRender()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	mMainVisualization->draw(mWorld);
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
