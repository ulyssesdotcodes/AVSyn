#pragma once

#include "cinder\audio\audio.h"
#include "cinder\gl\gl.h"

#include <vector>

using namespace ci;

class AudioSource {
public:
	void setup();
	std::vector<float> getMagSpectrum();
	gl::TextureRef getMagSpectrumTexture();

private:
	audio::InputDeviceNodeRef mInputNode;
	audio::MonitorSpectralNodeRef mMonitor;
};
