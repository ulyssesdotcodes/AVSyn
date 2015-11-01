#pragma once

#include "ShaderVisualization.h"
#include "PingPongFBO.h"

class Feedback : public ShaderVisualization {
public:
	Feedback(const string &fragment);
	virtual void update(const World& world) override;
	virtual void draw(const World& world) override;
	void switchParams(params::InterfaceGlRef params, const string &group);

private:
	gl::TextureRef mTexture;

	PingPongFBO mFbo;

	gl::GlslProgRef mUpdateShader;

	float mFade;
	float mLastTime;
};