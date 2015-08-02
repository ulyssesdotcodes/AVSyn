#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "ShaderVisualization.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class AVSynApp : public App {
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

private:
	ShaderVisualization mVisualization;
	AudioSource mAudioSource;
};

void AVSynApp::setup()
{
	mAudioSource.setup();
	mVisualization.setup(mAudioSource);
}

void AVSynApp::mouseDown( MouseEvent event )
{
}

void AVSynApp::update()
{
	mVisualization.update();
}

void AVSynApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
	gl::setMatricesWindow(getWindowSize());

	mVisualization.draw();
}

CINDER_APP(AVSynApp, RendererGl(), [&](App::Settings *settings) {
//settings->setFullScreen(true, FullScreenOptions());
	settings->setFrameRate(60.0f);
})
