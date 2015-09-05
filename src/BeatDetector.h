#pragma once

#include "AudioSource.h"
#include <vector>

const int HISTORY = 43;
const int BUCKETS = 20;

class BeatDetector {
public:
	BeatDetector(AudioSource* audioSource);
	void update(float c);
	float getBeat();

private:
	AudioSource* mAudioSource;

	array<array<float, HISTORY>, BUCKETS> mEnergyHistory;
	int mEnergyIndex;
	array<float, BUCKETS> mAverageEnergy;
	float mBeat;
	float mDeterioration;
	int mLastUpdate;
};