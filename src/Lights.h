#pragma once

#include "PingPongFBO.h"
#include "ShaderVisualization.h"

class Lights : public ShaderVisualization
{
public:
	Lights();
	virtual void update(const World& world) override;
	virtual void draw(const World& world) override;
	void switchParams(ci::params::InterfaceGlRef params, const std::string &group);

private:
	ci::gl::TextureRef mTexture;

	PingPongFBO mFbo;

	ci::gl::GlslProgRef mFadeShader;
	ci::gl::BatchRef mNewLight;

	float mFade;
	float mLastTime;
	ci::vec2 mResolution;
};
