#pragma once


#include "ShaderVisualization.h"
#include "PingPongFBO.h"

class Lights : public ShaderVisualization
{
public:
	void setup();
	virtual void update(const World& world) override;
	virtual void draw(const World& world) override;
	void switchParams(params::InterfaceGlRef params, const string &group);

private:
	gl::TextureRef mTexture;

	PingPongFBO mFbo;

	gl::GlslProgRef mFadeShader;
	gl::BatchRef mNewLight;

	float mFade;
	float mLastTime;
	vec2 mResolution;
};
