#pragma once

#include "ShaderVisualization.h"
#include "BeatDetector.h"

const int BIN_COUNT = 4;

class DotsVisualization : public ShaderVisualization {
public:
	void setup(AudioSource* audioSource, BeatDetector* beatDetector);

protected:
	virtual void renderUniforms() override;

private:
	float mAccumulatedLoudness;
	AudioSource* mAudioSource;
	BeatDetector* mBeatDetector;

	array<float, BIN_COUNT> mEqs;
};
