#include "AudioSource.h"

#include "cinder\audio\audio.h"
#include "cinder\gl\gl.h"
#include "cinder\gl\GlslProg.h"
#include "cinder\gl\Texture.h"
#include "cinder\app\App.h"
#include "cinder\Rand.h"

using namespace ci;
using namespace std;

void AudioSource::setup() {
	auto ctx = audio::Context::master();

	mInputNode = ctx->createInputDeviceNode();

	auto monitorFormat = audio::MonitorSpectralNode::Format().fftSize(2048).windowSize(1024);
	mMonitor = ctx->makeNode(new audio::MonitorSpectralNode(monitorFormat));

	mInputNode >> mMonitor;

	mInputNode->enable();
	ctx->enable();

	app::getWindow()->setTitle(mInputNode->getDevice()->getName());
}

vector<float> AudioSource::getMagSpectrum() {
	return mMonitor->getMagSpectrum();
}

gl::TextureRef AudioSource::getMagSpectrumTexture() {
	float spectrum[1024 * 4];
	vector<float> spectrumVec = getMagSpectrum();

	for (vector<float>::size_type i = 0; i < spectrumVec.size(); i++) {
		spectrum[i * 4] = audio::linearToDecibel(spectrumVec[i]) / 100.0f;
		spectrum[i * 4 + 1] = 0.0f;
		spectrum[i * 4 + 2] = 0.0f;
		spectrum[i * 4 + 3] = 256.0f;
	}

	return gl::Texture::create(Surface32f(spectrum, 1024, 1, 4, SurfaceChannelOrder::RGBA));
}