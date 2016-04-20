#include "Video.h"

using namespace ci;

Video::Video(const fs::path &moviePath)
{
	mVideo.loadMovie(moviePath);

	mGlslVideoTexture = gl::GlslProg::create( gl::GlslProg::Format()
		.vertex( app::loadAsset( "video_texture.vert" ) ) 
		.fragment( app::loadAsset( "video_texture.frag" ) ) );
}

void Video::update(const World& world)
{
	mVideo.update();
}

void Video::draw(const World& world)
{
	if(mVideo.isPlaying()) {
		vec2 videoSize = vec2( mVideo.getWidth(), mVideo.getHeight() );
		mGlslVideoTexture->uniform( "uVideoSize", videoSize );
		gl::clear(Color(0, 0, 0));
		gl::ScopedViewport vp(ivec2(0), world.windowSize);
		gl::pushMatrices();
		gl::setMatricesWindow(world.windowSize);
		gl::ScopedGlslProg glsl(mGlslVideoTexture);

		ciWMFVideoPlayer::ScopedVideoTextureBind scopedVideoTex( mVideo, 0 );
		gl::drawSolidRect(world.bounds);

		gl::popMatrices();
	}
}

void Video::switchParams(OscVisController &controller)
{
	mVideo.play();
}

void Video::hide()
{
	mVideo.pause();
}
