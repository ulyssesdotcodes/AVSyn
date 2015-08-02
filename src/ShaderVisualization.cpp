#include "ShaderVisualization.h"
#include "AudioSource.h"
#include "cinder\app\App.h"

ShaderVisualization::ShaderVisualization()
{
}

void ShaderVisualization::setup(const fs::path &fragmentShader ) 
{
	mShader = gl::GlslProg::create(app::loadAsset("passthru.vert"), app::loadAsset(fragmentShader));
}

void ShaderVisualization::update()
{
}

void ShaderVisualization::renderUniforms() {
	mShader->uniform("resolution", vec3(app::getWindowWidth(), app::getWindowHeight(), 0.0f));
	mShader->uniform("time", float(app::getElapsedSeconds()));
}

void ShaderVisualization::draw()
{
	gl::ScopedGlslProg glScp(mShader);

	renderUniforms();

	gl::drawSolidRect(app::getWindowBounds());
}