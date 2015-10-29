#include "Video.h"

void Video::setup(const fs::path &moviePath)
{
	mMovie = qtime::MovieGl::create(app::loadAsset(moviePath));
	mMovie->setLoop();
	mFrameTexture.reset();
}

void Video::update()
{
	if (mMovie) {
		if (!mMovie->isPlaying()) {
			mMovie->play();
		}

		mFrameTexture = mMovie->getTexture();
	}
}

void Video::draw()
{
	if (mFrameTexture) {
		Rectf centeredRect = Rectf( mFrameTexture->getBounds() ).getCenteredFit( app::getWindowIndex(0)->getBounds(), true );
		gl::draw( mFrameTexture, centeredRect );
	}
}

bool Video::perspective()
{
	return false;
}

void Video::switchCamera(ci::CameraPersp * cam)
{
	mMovie->play();
}

void Video::switchParams(ci::params::InterfaceGlRef params, const string & group)
{
}

void Video::resetParams(ci::params::InterfaceGlRef params)
{
	Visualization::resetParams(params);
	mMovie->stop();
}
