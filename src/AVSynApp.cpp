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

};

void AVSynApp::setup()
{
}

void AVSynApp::mouseDown( MouseEvent event )
{
}

void AVSynApp::update()
{
}

void AVSynApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( AVSynApp, RendererGl )
