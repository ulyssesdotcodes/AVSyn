#include "DotsVisualization.h"

const float DAMPING = 0.1;

DotsVisualization::DotsVisualization() : ShaderVisualization("dots.frag")
{
	mLoudness = 1.0;
	mAccumulatedLoudness = 0.0f;
}

void DotsVisualization::renderUniforms(const World& world)
{
	ShaderVisualization::renderUniforms(world);

	world.audioSource->update();
	world.beatDetector->update(world, 1.4);

	mAccumulatedLoudness += world.beatDetector->getBeat() + world.audioSource->getVolume() * mLoudness;

	std::vector<float> eqs = world.audioSource->getEqs(BIN_COUNT);

	for (int i = 0; i < BIN_COUNT; ++i) {
		if (eqs[i] > mEqs[i]) {
			mEqs[i] = eqs[i] * mLoudness;
		}
		else {
			mEqs[i] -= (mEqs[i] - eqs[i]) * DAMPING;
		}
	}

	mShader->uniform("i_eqs", &mEqs[0], BIN_COUNT);
	mShader->uniform("i_accumulatedLoudness", mAccumulatedLoudness);
}

void DotsVisualization::switchParams(OscVisController &controller) {
	controller.subscribeSliderListener("Loudness", 0, 2, [&](float val) { mLoudness = val; });
}
