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
};

void AVSynApp::setup()
{
	getWindow()->getSignalDraw().connect([=]() { drawRender(); });
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

	//AudioShaderVisualization *simpleVis = new AudioShaderVisualization();
	//simpleVis->setup(mAudioSource, "simple.frag");
	//mVisualizations.insert(make_pair("Simple", simpleVis));
	//mVisualizationOptions.push_back("Simple");

	AudioShaderVisualization *circularVis = new AudioShaderVisualization();
	circularVis->setup(mAudioSource, "circular_fft.frag");
	mVisualizations.insert(make_pair("Circular", circularVis));
	mVisualizationOptions.push_back("Circular");

	AudioShaderVisualization *flame = new AudioShaderVisualization();
	flame->setup(mAudioSource, "flame.frag");
	mVisualizations.insert(make_pair("Flame", flame));
	mVisualizationOptions.push_back("Flame");

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

	auto *trees = new TreeVisualization();
	trees->setup(mAudioSource, mBeatDetector);
	mVisualizations.insert(make_pair("Trees", trees));
	mVisualizationOptions.push_back("Trees");

	auto *kickChangeImage = new KinectParticles();
	kickChangeImage->setup(mAudioSource, mBeatDetector, mVisualizations, mVisualizationOptions);
	mVisualizations.insert(make_pair("Kick Change Image", kickChangeImage));
	mVisualizationOptions.push_back("Kick Change Image");

	getWindowIndex(0)->getSignalMouseMove().connect([kickChangeImage](MouseEvent e) {
		app::getWindowIndex(0)->getRenderer()->makeCurrentContext();
		kickChangeImage->mouseMove(e);
	});

	mCurrentVisOption = 2;
	mVisualization = mVisualizations[mVisualizationOptions[mCurrentVisOption]];
	
	mVisualization->switchCamera(&mCam);
	mVisualization->switchParams(mParams);

	mParams->addParam("Visualizations", mVisualizationOptions, 
		[=](int ind) {
			mVisualization->resetParams(mParams);
			mCurrentVisOption = ind;
			mVisualization = mVisualizations[mVisualizationOptions[mCurrentVisOption]];
			mVisualization->switchCamera(&mCam);
			mVisualization->switchParams(mParams);
		},
		[=]() {
			return mCurrentVisOption;
		}
		);
}

void AVSynApp::keyDown(KeyEvent event) {
	if (event.getChar() == 'q') {
		quit();
	}
}

void AVSynApp::update()
{
	getWindowIndex(0)->getRenderer()->makeCurrentContext();
	mDeltaSource->update();
	mVisualization->update();
}

void AVSynApp::drawRender()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	if (mVisualization->perspective()) {
		gl::setMatrices(mCam);
		gl::rotate(mSceneRotation);
		gl::enableDepthRead();
		gl::enableDepthWrite();
	}
	else {
		gl::setMatricesWindow(getWindowSize());
	}

	mVisualization->draw();

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
		options.display(displays[1]);
		settings->setDisplay(displays[1]);
	}
	settings->setFrameRate(60.0f);
})
