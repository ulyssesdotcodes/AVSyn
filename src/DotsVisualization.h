#pragma once

#include "ShaderVisualization.h"
#include "BeatDetector.h"

const int BIN_COUNT = 4;

/*
  A shader visualization that represents different parts of the audio spectrum as dots.
*/
class DotsVisualization : public ShaderVisualization {
public:
	//! Setup both the audio source and a beat detector.
	void setup(AudioSource* audioSource, BeatDetector* beatDetector);

protected:
	//! Update the beat detector, grab eq values for the audio spectrum, and update the shader uniforms appropriately.
	virtual void renderUniforms() override;

	//! Control hue and loudness
	virtual void switchParams(params::InterfaceGlRef params, const string &group) override;

private:
	AudioSource* mAudioSource;
	BeatDetector* mBeatDetector;

	array<float, BIN_COUNT> mEqs;

	float mAccumulatedLoudness;

	float mLoudness;
	float mHue;
};
