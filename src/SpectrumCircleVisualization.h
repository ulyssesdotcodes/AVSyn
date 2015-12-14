#pragma once

#include "AudioShaderVisualization.h"

class SpectrumCircleVisualization : public AudioShaderVisualization {
public:
	SpectrumCircleVisualization();

	virtual void switchParams(OscVisController &controller) override;
	virtual void renderUniforms(const World& world) override;

private:
	float mVolume;
	float mHoleSize;
};