#include "ShaderVisualization.h"
#include "AudioSource.h"
#include "cinder\app\App.h"

ShaderVisualization::ShaderVisualization()
{
}

void ShaderVisualization::setup(AudioSource audioSource) 
{
	mAudioSource = audioSource;

	mShader = gl::GlslProg::create(app::loadAsset("passthru.vert"), app::loadAsset("simple.frag"));
}

void ShaderVisualization::update()
{
	mTexture = mAudioSource.getMagSpectrumTexture();

}

void ShaderVisualization::draw()
{
	gl::ScopedGlslProg glScp(mShader);
	gl::ScopedTextureBind texscp(mTexture);
	mShader->uniform("iResolution", vec3(app::getWindowWidth(), app::getWindowHeight(), 0.0f));
	mShader->uniform("iGlobalTime", float(app::getElapsedSeconds()));
	mShader->uniform("iChannel0", 0);

	gl::drawSolidRect(app::getWindowBounds());
}