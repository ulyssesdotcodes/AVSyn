#include "DotsVisualization.h"

#include <vector>

using namespace std;

const float DAMPING = 0.1;

void DotsVisualization::setup(AudioSource* audioSource, BeatDetector* beatDetector)
{
	ShaderVisualization::setup("dots.frag");

	mAccumulatedLoudness = 0.0f;
	mAudioSource = audioSource;
	mBeatDetector = beatDetector;
}

void DotsVisualization::renderUniforms()
{
	ShaderVisualization::renderUniforms();

	mAudioSource->update();
	mBeatDetector->update(1.5);
	mAccumulatedLoudness += mBeatDetector->getBeat() + mAudioSource->getVolume();

	vector<float> eqs = mAudioSource->getEqs(BIN_COUNT);

	for (int i = 0; i < BIN_COUNT; ++i) {
		if (eqs[i] > mEqs[i]) {
			mEqs[i] = eqs[i];
		}
		else {
			mEqs[i] -= (mEqs[i] - eqs[i]) * DAMPING;
		}
	}


	mShader->uniform("eqs", &mEqs[0], BIN_COUNT);
	mShader->uniform("accumulatedLoudness", mAccumulatedLoudness);
}
