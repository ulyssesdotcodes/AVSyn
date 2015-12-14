#pragma once

#include "PingPongFBO.h"
#include "ShaderVisualization.h"

class Feedback : public Visualization {
public:
	Feedback(const std::string &fragment);
	virtual void update(const World& world) override;
	virtual void draw(const World& world) override;
	void switchParams(OscVisController &controller);

private:
	ci::gl::TextureRef mTexture;

	ci::gl::GlslProgRef mUpdateShader;

	float mLastTime;
	float mVolume;
};