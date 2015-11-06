#include "ShaderVisualization.h"

#include "cinder/app/App.h"

#include "AudioSource.h"

using namespace ci;

ShaderVisualization::ShaderVisualization(const fs::path &fragmentShader )
{
	mShader = gl::GlslProg::create(app::loadAsset("passthru.vert"), app::loadAsset(fragmentShader));

	mShader->uniform("i_resolution", vec2(app::getWindowIndex(0)->getSize()));
}

void ShaderVisualization::update(const World& world)
{
}

void ShaderVisualization::renderUniforms(const World& world) {
	mShader->uniform("i_time", float(app::getElapsedSeconds()));
}

void ShaderVisualization::draw(const World& world)
{
	gl::clear(Color(0, 0, 0));
	gl::setMatricesWindow(world.windowSize);

	gl::ScopedGlslProg glScp(mShader);

	renderUniforms(world);

	gl::drawSolidRect(world.bounds);
}