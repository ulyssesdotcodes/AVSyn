#pragma once

#include "ShaderVisualization.h"

/*
  Renders a shader that requires the current audio frequency spectrum as a texture to the screen.
*/
class AudioShaderVisualization : public ShaderVisualization {
public:
	//! Setup with an audio source and an audio fragment shader
	void setup(AudioSource* audioSource, const fs::path &fragmentShader);
	//! Update the audio source and texture
	virtual void update() override;
	//! Set the texture as a uniform and draw the shader.
	virtual void draw() override;
	//! Nullop
	virtual void switchParams(params::InterfaceGlRef params, const string &group) override;

protected:
	virtual void renderUniforms() override;
	AudioSource* mAudioSource;

private:
	gl::TextureRef mTexture;
	float mHue;
	float mVolume;
	float mCycleHueSpeed;
};