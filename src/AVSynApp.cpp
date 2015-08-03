#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder\params\Params.h"
#include "cinder\Camera.h"
#include "boost\range\adaptor\map.hpp"
#include "AudioShaderVisualization.h"
#include "ShaderVisualization.h"
#include "DotsVisualization.h"
#include "EQPointCloud.h"
#include "Visualization.h"

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

	CameraPersp mCam;
	vec3 mEye;
	vec3 mCenter;
	vec3 mUp;

	Visualization *mVisualization;
	AudioSource mAudioSource;

	map<string, Visualization*> mVisualizations;
	vector<string> mVisualizationOptions;
	int mCurrentVisOption;
};

void AVSynApp::setup()
{
	getWindow()->getSignalDraw().connect([=]() { drawRender(); });

	mCam = CameraPersp(getWindowWidth(), getWindowHeight(), 50);
	mCam.setPerspective(60.0f, getWindowAspectRatio(), 5.0f, 3000.0f);
	mEye = vec3(0.0f, 0.0f, 500.0f);
	mCenter = vec3(0.0f);
	mUp = vec3(0.0f, 1.0f, 0.0f);

	vec2 paramsSize = vec2(255, 200);
	mCurrentVisOption = 0;
	auto format = Window::Format();
	format.setSize(paramsSize + vec2(40, 40));
	mParamWindow = createWindow(format);
	mParamWindow->setPos(vec2(0, 0));
	mParamWindow->getSignalDraw().connect([=]() { drawParams(); });
	mParams = params::InterfaceGl::create(mParamWindow, "Options", paramsSize);

	mParams->addParam("Rotation", &mSceneRotation);

	mAudioSource.setup();

	AudioShaderVisualization *simpleVis = new AudioShaderVisualization();
	simpleVis->setup(mAudioSource, "simple.frag");
	mVisualizations.insert(make_pair("Simple", simpleVis));
	mVisualizationOptions.push_back("Simple");

	AudioShaderVisualization *circularVis = new AudioShaderVisualization();
	circularVis->setup(mAudioSource, "circular_fft.frag");
	mVisualizations.insert(make_pair("Circular", circularVis));
	mVisualizationOptions.push_back("Circular");

	auto *dotsVis = new DotsVisualization();
	dotsVis->setup(mAudioSource);
	mVisualizations.insert(make_pair("Dots", dotsVis));
	mVisualizationOptions.push_back("Dots");

	auto *eqPointCloud = new EQPointCloud();
	eqPointCloud->setup(mAudioSource);
	mVisualizations.insert(make_pair("EQPointCloud", eqPointCloud));
	mVisualizationOptions.push_back("EQPointCloud");

	mCurrentVisOption = mVisualizations.size() - 1;
	mVisualization = eqPointCloud;

	mParams->addParam("Visualizations", mVisualizationOptions, 
		[=](int ind) {
			mCurrentVisOption = ind;
			mVisualization = mVisualizations[mVisualizationOptions[mCurrentVisOption]];
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
	mVisualization->update();

	mCam.lookAt(mEye, mCenter, mUp);
}

void AVSynApp::drawRender()
{
	gl::enableDepthRead();
	gl::enableDepthWrite();

	gl::setMatrices(mCam);
	gl::rotate(mSceneRotation);

	mVisualization->draw();
}

void AVSynApp::drawParams()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	mParams->draw();
}

CINDER_APP(AVSynApp, RendererGl(), [&](App::Settings *settings) {
//settings->setFullScreen(true, FullScreenOptions());
	settings->setFrameRate(60.0f);
})
