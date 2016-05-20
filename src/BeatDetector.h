#pragma once

#include "AudioSource.h"
#include "World.h"

const int HISTORY = 43;
const int BUCKETS = 16;

struct World;

class BeatDetector {
public:
	BeatDetector();
	void update(const World& world, float c);
	float getBeat();

private:
	std::array<std::array<float, HISTORY>, BUCKETS> mEnergyHistory;
	int mEnergyIndex;
	std::array<float, BUCKETS> mAverageEnergy;
	float mBeat;
	float mDeterioration;
	int mLastUpdate;
};