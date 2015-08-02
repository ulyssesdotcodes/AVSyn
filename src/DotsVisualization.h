#pragma once

#include "ShaderVisualization.h"

class DotsVisualization : public ShaderVisualization {
public:
	void setup(AudioSource audioSource);

protected:
	virtual void renderUniforms() override;

private:
	int mBinCount;
	float mAccumulatedLoudness;
};
