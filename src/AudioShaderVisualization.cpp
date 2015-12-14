#include "AudioShaderVisualization.h"

#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

using namespace ci;

AudioShaderVisualization::AudioShaderVisualization(const fs::path &fragmentShader) : ShaderVisualization(fragmentShader)
{
}

void AudioShaderVisualization::switchParams(OscVisController &controller)
{
	controller.subscribeSliderGlslListener("Volume", 0, 1, 1, mShader, "i_volume");
}

void AudioShaderVisualization::update(const World& world)
{
	world.audioSource->update();
	mTexture = world.audioSource->getMagSpectrumTexture();
}

void AudioShaderVisualization::renderUniforms(const World& world)
{
	mShader->uniform("tex_audio", 0);
	ShaderVisualization::renderUniforms(world);
}

void AudioShaderVisualization::draw(const World& world)
{
	gl::ScopedTextureBind texscp(mTexture);
	ShaderVisualization::draw(world);
}