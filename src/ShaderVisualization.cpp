#include "ShaderVisualization.h"
#include "AudioSource.h"
#include "cinder\app\App.h"

ShaderVisualization::ShaderVisualization()
{
}

void ShaderVisualization::setup(const fs::path &fragmentShader ) 
{
	mShader = gl::GlslProg::create(app::loadAsset("passthru.vert"), app::loadAsset(fragmentShader));

	mShader->uniform("resolution", vec3(app::getWindowIndex(0)->getWidth(), app::getWindowIndex(0)->getHeight(), 0.0f));
}

void ShaderVisualization::update()
{
}

void ShaderVisualization::switchCamera(CameraPersp* cam) {}

void ShaderVisualization::renderUniforms() {
	mShader->uniform("time", float(app::getElapsedSeconds()));
}

void ShaderVisualization::draw()
{
	gl::ScopedGlslProg glScp(mShader);

	renderUniforms();

	gl::drawSolidRect(app::getWindowIndex(0)->getBounds());
}

bool ShaderVisualization::perspective()
{
	return false;
}