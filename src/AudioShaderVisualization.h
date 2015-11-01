#pragma once

#include "ShaderVisualization.h"

/*
  Renders a shader that requires the current audio frequency spectrum as a texture to the screen.
*/
class AudioShaderVisualization : public ShaderVisualization {
public:
	//! Setup with an audio source and an audio fragment shader
	AudioShaderVisualization(const fs::path &fragmentShader);
	//! Update the audio source and texture
	virtual void update(const World& world) override;
	//! Set the texture as a uniform and draw the shader.
	virtual void draw(const World& world) override;
	//! Nullop
	virtual void switchParams(params::InterfaceGlRef params, const string &group) override;

protected:
	virtual void renderUniforms(const World& world) override;

private:
	gl::TextureRef mTexture;
	float mHue;
	float mVolume;
	float mCycleHueSpeed;
};