#include "BeatDetector.h"

#include "cinder/app/App.h"
#include "cinder/CinderMath.h"

using namespace ci;

const float VOLUME_MIN = 0.001;

BeatDetector::BeatDetector() {
	mBeat = 0;
	mEnergyIndex = 0;
	mDeterioration = 0;

	mAverageEnergy = {};
	for (int i = 0; i < BUCKETS; ++i) {
		mEnergyHistory[i] = {};
	}
}

void BeatDetector::update(const World& world, float c) {
	if (world.audioSource->getVolume() < VOLUME_MIN) {
		mBeat -= mDeterioration;
		mBeat = math<float>::max(mBeat, 0.0);
		return;
	}

	int frames = app::getElapsedFrames();
	if (mLastUpdate == frames) {
		return;
	}

	mLastUpdate = frames;

	std::array<float, BUCKETS> sum = {};
	int j = 0;
	int startBucketIndex = 0;
	int bucketSize = 1;
	int i = 0;
	world.audioSource->update();
	std::vector<float> spectrum = world.audioSource->getMagSpectrum();
	while (j < BUCKETS) {
		sum[j] += spectrum[i] / bucketSize;
		i++;

		if (i >= startBucketIndex + bucketSize) {
			j++;
			bucketSize = j + 1;
			startBucketIndex = i;
		}
	}

	float beat = -1.0;
	for (int i = 0; i < BUCKETS; ++i) {
		if (beat < 0) {
			beat = sum[i] - c * mAverageEnergy[i];
			if (beat > 0) {
				beat = 1.0;
			}
		}
		mAverageEnergy[i] -=
			mEnergyHistory[i][mEnergyIndex] / HISTORY;
		mEnergyHistory[i][mEnergyIndex] = sum[i];
		mAverageEnergy[i] += mEnergyHistory[i][mEnergyIndex] / HISTORY;
		++mEnergyIndex;
		if (mEnergyIndex >= HISTORY) {
			mEnergyIndex = 0;
		}
	}

	if (beat > mBeat) {
		mBeat = beat;
		mDeterioration = 4 * beat / HISTORY;
	}
	else {
		mBeat -= mDeterioration;
		mBeat = math<float>::max(mBeat, 0.0);
	}
}

float BeatDetector::getBeat() {
	return mBeat;
}