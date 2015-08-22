#include "AudioShaderVisualization.h"

#include "cinder\gl\gl.h"
#include "cinder\app\App.h"

void AudioShaderVisualization::setup(AudioSource* audioSource, const fs::path &fragmentShader)
{
	ShaderVisualization::setup(fragmentShader);
	mAudioSource = audioSource;
	mHue = 0;
}

void AudioShaderVisualization::switchParams(params::InterfaceGlRef params)
{
	addParamName("Hue");
	params->addParam("Hue", &mHue, "min=0.0 max=1.0 step=0.01");
}

void AudioShaderVisualization::update()
{
	mAudioSource->update();
	mTexture = mAudioSource->getMagSpectrumTexture();
}

void AudioShaderVisualization::renderUniforms()
{
	mShader->uniform("audioTexture", 0);
	mShader->uniform("hue", mHue);
	ShaderVisualization::renderUniforms();
}

void AudioShaderVisualization::draw()
{
	gl::ScopedGlslProg glScp(mShader);
	gl::ScopedTextureBind texscp(mTexture);

	renderUniforms();

	gl::drawSolidRect(app::getWindowBounds());
}