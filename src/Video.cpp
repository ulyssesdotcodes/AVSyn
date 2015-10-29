#include "Video.h"

void Video::setup(const fs::path &moviePath)
{
	mMovie = qtime::MovieGl::create(app::loadAsset(moviePath));
	mFrameTexture.reset();
}

void Video::update()
{
	if (mMovie) {
		mFrameTexture = mMovie->getTexture();
	}
}

void Video::draw()
{
	if (mFrameTexture) {
		Rectf centeredRect = Rectf( mFrameTexture->getBounds() ).getCenteredFit( app::getWindowBounds(), true );
		gl::draw( mFrameTexture, centeredRect );
	}
}

bool Video::perspective()
{
	return false;
}

void Video::switchCamera(ci::CameraPersp * cam)
{
	mMovie->setLoop();
	mMovie->play();
}

void Video::switchParams(ci::params::InterfaceGlRef params, const string & group)
{
}
