#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Surface.h"

#include "cinder/Filesystem.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder\qtime\QuickTimeGl.h"

#include "Visualization.h"

using namespace ci;

class Video : public Visualization
{
public: 
	//! Setup the video
	void setup(const fs::path &moviePath);

	//! Perform all computations for the visualization.
	virtual void update();
	//! Draw the visualization to the screen. This assumes matrices and viewport have been set up already.
	virtual void draw();
	//! Defines whether this should be rendered with perspective or or orthogonally.
	virtual bool perspective();
	//! Set up the camera for this visualization when the user switches to it.
	virtual void switchCamera(ci::CameraPersp* cam);
	//! Set up the parameters for this visualization when the user switches to it.
	virtual void switchParams(ci::params::InterfaceGlRef params, const string &group);

private:
	gl::TextureRef			mFrameTexture;
	qtime::MovieGlRef		mMovie;
};