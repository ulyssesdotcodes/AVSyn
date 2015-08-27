#include "AudioShaderVisualization.h"

#include "cinder\gl\gl.h"
#include "cinder\app\App.h"

void AudioShaderVisualization::setup(AudioSource* audioSource, const fs::path &fragmentShader)
{
	ShaderVisualization::setup(fragmentShader);
	mAudioSource = audioSource;
	mHue = 0;
	mCycleHueSpeed = 0.0;
	mVolume = 1.0;
}

void AudioShaderVisualization::switchParams(params::InterfaceGlRef params)
{
	addParamName("Hue");
	params->addParam("Hue", &mHue, "min=0.0 max=1.0 step=0.01");

	addParamName("Volume");
	params->addParam("Volume", &mVolume, "min=0.0 max=3.0 step=0.01");

	addParamName("Cycle Hue Speed");
	params->addParam("Cycle Hue Speed", &mCycleHueSpeed, "min=0.0 max=0.01667 step=0.0001");
}

void AudioShaderVisualization::update()
{
	mAudioSource->update();
	mTexture = mAudioSource->getMagSpectrumTexture();

	mHue += glm::fract(mHue + mCycleHueSpeed);
}

void AudioShaderVisualization::renderUniforms()
{
	mShader->uniform("audioTexture", 0);
	mShader->uniform("hue", mHue);
	mShader->uniform("volume", mVolume);
	ShaderVisualization::renderUniforms();
}

void AudioShaderVisualization::draw()
{
	gl::ScopedGlslProg glScp(mShader);
	gl::ScopedTextureBind texscp(mTexture);

	renderUniforms();

	gl::drawSolidRect(app::getWindowBounds());
}