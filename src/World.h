#pragma once

#include "cinder/Camera.h"

#include "AudioSource.h"
#include "BeatDetector.h"
#include "DeltaSource.h"

class BeatDetector;

struct World {
	std::unique_ptr<ci::CameraPersp> camera;
	std::unique_ptr<AudioSource> audioSource;
	std::unique_ptr<DeltaSource> deltaSource;
	std::unique_ptr<BeatDetector> beatDetector;
	ci::ivec2 windowSize;
	ci::Area bounds;
};
