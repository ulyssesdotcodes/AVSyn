#pragma once

#include "cinder\Camera.h"

#include "AudioSource.h"
#include "DeltaSource.h"
#include "BeatDetector.h"

class BeatDetector;

struct World {
	unique_ptr<CameraPersp> camera;
	unique_ptr<AudioSource> audioSource;
	unique_ptr<DeltaSource> deltaSource;
	unique_ptr<BeatDetector> beatDetector;
	ivec2 windowSize;
	Area bounds;
};
