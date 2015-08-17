#pragma once

#include "ShaderVisualization.h"

class AudioShaderVisualization : public ShaderVisualization {
public:
	void setup(AudioSource* audioSource, const fs::path &fragmentShader);
	virtual void update() override;
	virtual void draw() override;
	virtual void switchParams(params::InterfaceGlRef params);

protected:
	virtual void renderUniforms() override;
	AudioSource* mAudioSource;

private:
	gl::TextureRef mTexture;
};