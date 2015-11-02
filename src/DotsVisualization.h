#pragma once

#include "BeatDetector.h"
#include "ShaderVisualization.h"

const int BIN_COUNT = 4;

/*
  A shader visualization that represents different parts of the audio spectrum as dots.
*/
class DotsVisualization : public ShaderVisualization {
public:
	//! Setup both the audio source and a beat detector.
	DotsVisualization();

protected:
	//! Update the beat detector, grab eq values for the audio spectrum, and update the shader uniforms appropriately.
	virtual void renderUniforms(const World& world) override;

	//! Control hue and loudness
	virtual void switchParams(ci::params::InterfaceGlRef params, const std::string &group) override;

private:
	std::array<float, BIN_COUNT> mEqs;

	float mAccumulatedLoudness;

	float mLoudness;
	float mHue;
};
