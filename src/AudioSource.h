#pragma once

#include "cinder\audio\audio.h"
#include "cinder\gl\gl.h"

#include <vector>

using namespace ci;
using namespace std;

class AudioSource {
public:
	void setup();
	void update();
	vector<float> getMagSpectrum();
	gl::TextureRef getMagSpectrumTexture();
	float getVolume();
	vector<float> getEqs(int binCount);
	vector<float> getEqs(int binCount, float scale);

private:
	audio::InputDeviceNodeRef mInputNode;
	audio::MonitorSpectralNodeRef mMonitor;

	vector<float>  mSpectrum;

	int mLastUpdateFrame;
};
