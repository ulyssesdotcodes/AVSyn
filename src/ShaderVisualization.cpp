#include "ShaderVisualization.h"
#include "AudioSource.h"
#include "cinder\app\App.h"

ShaderVisualization::ShaderVisualization()
{
}

void ShaderVisualization::setup(AudioSource audioSource, const fs::path &fragmentShader ) 
{
	mAudioSource = audioSource;

	mShader = gl::GlslProg::create(app::loadAsset("passthru.vert"), app::loadAsset(fragmentShader));
}

void ShaderVisualization::update()
{
	mTexture = mAudioSource.getMagSpectrumTexture();

}

void ShaderVisualization::draw()
{
	gl::ScopedGlslProg glScp(mShader);
	gl::ScopedTextureBind texscp(mTexture);
	mShader->uniform("resolution", vec3(app::getWindowWidth(), app::getWindowHeight(), 0.0f));
	mShader->uniform("time", float(app::getElapsedSeconds()));
	mShader->uniform("audioTexture", 0);

	gl::drawSolidRect(app::getWindowBounds());
}