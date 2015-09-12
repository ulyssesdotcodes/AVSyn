#pragma once

#include "ShaderVisualization.h"
#include "PingPongFBO.h"

class Feedback : public ShaderVisualization {
public:
	void setup(AudioSource* audioSource, const string &fragment);
	virtual void update() override;
	virtual void draw() override;
	void switchParams(params::InterfaceGlRef params, const string &group);

private:
	AudioSource* mAudioSource;
	gl::TextureRef mTexture;

	PingPongFBO mFbo;

	gl::GlslProgRef mUpdateShader;

	float mFade;
};