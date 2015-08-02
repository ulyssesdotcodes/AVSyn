#include "DotsVisualization.h"

#include "AudioUtils.h"

void DotsVisualization::setup(AudioSource audioSource)
{
	ShaderVisualization::setup(audioSource, "dots.frag");

	mBinCount = 4;
	mAccumulatedLoudness = 0.0f;
}

void DotsVisualization::renderUniforms()
{
	ShaderVisualization::renderUniforms();

	mAccumulatedLoudness += mAudioSource.getVolume();

	vector<float> eqs = audioUtils::eqs(mAudioSource.getMagSpectrum(), mBinCount);

	mShader->uniform("eqs", &eqs[0], mBinCount);
	mShader->uniform("accumulatedLoudness", mAccumulatedLoudness);
}




