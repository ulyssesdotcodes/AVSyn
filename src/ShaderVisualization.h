#pragma once

#include "cinder\gl\gl.h"
#include "AudioSource.h"
#include "Visualization.h"

using namespace ci;

/*
  A visualization that renders a glsl fragment shader to the screen.
*/
class ShaderVisualization : public Visualization {
public:
	ShaderVisualization();
	//! Setup the fragment shader program.
	void setup(const fs::path &fragmentShader);

	//! Nullops
	virtual void update() override;
	virtual void switchCamera(CameraPersp* cam) override;

	//! Draw the fragment shader to the screen. Sets resolution and time uniforms.
	virtual void draw() override;

	//! Returns false
	bool perspective() override;

protected:
	//! Convenience method for child classes to override to add uniforms to the shader program.
	virtual void renderUniforms();
	gl::GlslProgRef mShader;
};
