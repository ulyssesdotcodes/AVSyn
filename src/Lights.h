#pragma once

#include "PingPongFBO.h"
#include "ShaderVisualization.h"

class Lights : public ShaderVisualization
{
public:
	Lights();
	virtual void update(const World& world) override;
	virtual void draw(const World& world) override;
	void switchParams(OscVisController &controller);

private:
	ci::gl::BatchRef mNewLight;
	ci::vec2 mResolution;
	float mFrequency;
};
