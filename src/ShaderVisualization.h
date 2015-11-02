#pragma once

#include "cinder/gl/gl.h"

#include "AudioSource.h"
#include "Visualization.h"

/*
  A visualization that renders a glsl fragment shader to the screen.
*/
class ShaderVisualization : public Visualization {
public:
	//! Setup the fragment shader program.
	ShaderVisualization(const ci::fs::path &fragmentShader);

	//! Nullops
	virtual void update(const World& world) override;
	//! Draw the fragment shader to the screen. Sets resolution and time uniforms.
	virtual void draw(const World& world) override;

protected:
	//! Convenience method for child classes to override to add uniforms to the shader program.
	virtual void renderUniforms(const World& world);
	ci::gl::GlslProgRef mShader;
};
