#include "DotsVisualization.h"

#include <vector>

using namespace std;

void DotsVisualization::setup(AudioSource audioSource)
{
	ShaderVisualization::setup("dots.frag");

	mBinCount = 4;
	mAccumulatedLoudness = 0.0f;
	mAudioSource = audioSource;
}

void DotsVisualization::renderUniforms()
{
	ShaderVisualization::renderUniforms();

	mAccumulatedLoudness += mAudioSource.getVolume();

	vector<float> eqs = mAudioSource.getEqs(mBinCount);

	mShader->uniform("eqs", &eqs[0], mBinCount);
	mShader->uniform("accumulatedLoudness", mAccumulatedLoudness);
}




