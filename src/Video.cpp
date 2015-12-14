#include "Video.h"

using namespace ci;

Video::Video(const fs::path &moviePath)
{
	mMovie = qtime::MovieGl::create(app::loadAsset(moviePath));
	mMovie->setLoop();
	mFrameTexture.reset();
}

void Video::update(const World& world)
{
	if (mMovie) {
		if (!mMovie->isPlaying()) {
			mMovie->play();
		}

		mFrameTexture = mMovie->getTexture();
	}
}

void Video::draw(const World& world)
{
	gl::setMatricesWindow(world.windowSize);

	if (mFrameTexture) {
		Rectf centeredRect = Rectf( mFrameTexture->getBounds() ).getCenteredFit( app::getWindowIndex(0)->getBounds(), true );
		gl::draw( mFrameTexture, centeredRect );
	}
}

void Video::switchParams(OscVisController &controller)
{
}

void Video::hide()
{
	mMovie->stop();
}
