#pragma once

#include "AudioShaderVisualization.h"

class SpectrumCircleVisualization : public AudioShaderVisualization {
public:
	SpectrumCircleVisualization();

	virtual void switchParams(ci::params::InterfaceGlRef params, const std::string &group) override;
	virtual void renderUniforms(const World& world) override;

private:
	float mVolume;
	float mHoleSize;
};