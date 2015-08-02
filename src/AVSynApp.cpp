#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder\params\Params.h"
#include "boost\range\adaptor\map.hpp"
#include "AudioShaderVisualization.h"
#include "ShaderVisualization.h"
#include "DotsVisualization.h"
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
	Visualization *mVisualization;
	AudioSource mAudioSource;

	WindowRef mParamWindow;
	params::InterfaceGlRef mParams;

	map<string, Visualization*> mVisualizations;
	vector<string> mVisualizationOptions;
	int mCurrentVisOption;
};

void AVSynApp::setup()
{
	getWindow()->getSignalDraw().connect([=]() { drawRender(); });

	vec2 paramsSize = vec2(255, 200);
	mCurrentVisOption = 0;
	auto format = Window::Format();
	format.setSize(paramsSize + vec2(40, 40));
	mParamWindow = createWindow(format);
	mParamWindow->setPos(vec2(0, 0));
	mParamWindow->getSignalDraw().connect([=]() { drawParams(); });
	mParams = params::InterfaceGl::create(mParamWindow, "Options", paramsSize);

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


	mParams->addParam("Visualizations", mVisualizationOptions, 
		[=](int ind) {
			mCurrentVisOption = ind;
			mVisualization = mVisualizations[mVisualizationOptions[mCurrentVisOption]];
		},
		[=]() {
			return mCurrentVisOption;
		}
		);

	mVisualization = simpleVis;
}

void AVSynApp::keyDown(KeyEvent event) {
	if (event.getChar() == 'q') {
		quit();
	}
}

void AVSynApp::update()
{
	mVisualization->update();
}

void AVSynApp::drawRender()
{
	gl::setMatricesWindow(getWindowSize());

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
