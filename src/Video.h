#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Filesystem.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"

#include "ciWMFVideoPlayer.h"

#include "Visualization.h"

class Video : public Visualization
{
public: 
	//! Setup the video
	Video(const ci::fs::path &moviePath);

	//! Perform all computations for the visualization.
	virtual void update(const World& world);
	//! Draw the visualization to the screen. This assumes matrices and viewport have been set up already.
	virtual void draw(const World& world);
	//! Set up the parameters for this visualization when the user switches to it.
	virtual void switchParams(OscVisController &controller);
	virtual void hide();

private:
	ciWMFVideoPlayer mVideo;

	ci::gl::GlslProgRef			mGlslVideoTexture;
};