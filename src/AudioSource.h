#pragma once

#include "cinder\audio\audio.h"
#include "cinder\gl\gl.h"

#include <vector>

using namespace ci;
using namespace std;

class AudioSource {
public:
	void setup();
	vector<float> getMagSpectrum();
	gl::TextureRef getMagSpectrumTexture();
	float getVolume();
	vector<float> getEqs(int binCount);

private:
	audio::InputDeviceNodeRef mInputNode;
	audio::MonitorSpectralNodeRef mMonitor;

};
