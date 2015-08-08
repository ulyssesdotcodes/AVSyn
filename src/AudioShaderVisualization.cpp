#include "AudioShaderVisualization.h"

#include "cinder\gl\gl.h"
#include "cinder\app\App.h"

void AudioShaderVisualization::setup(AudioSource* audioSource, const fs::path &fragmentShader)
{
	ShaderVisualization::setup(fragmentShader);
	mAudioSource = audioSource;
}

void AudioShaderVisualization::update()
{
	mAudioSource->update();
	mTexture = mAudioSource->getMagSpectrumTexture();
}

void AudioShaderVisualization::renderUniforms()
{
	mShader->uniform("audioTexture", 0);
	ShaderVisualization::renderUniforms();
}

void AudioShaderVisualization::draw()
{
	gl::ScopedGlslProg glScp(mShader);
	gl::ScopedTextureBind texscp(mTexture);

	renderUniforms();

	gl::drawSolidRect(app::getWindowBounds());
}