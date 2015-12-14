#include "Feedback.h"

#include "cinder/app/App.h"

using namespace ci;

Feedback::Feedback(const std::string &fragment)
{
	vec2 resolution = app::getWindowIndex(0)->getSize();

	gl::GlslProg::Format shaderFmt;
	shaderFmt.vertex(app::loadAsset("passthru.vert"))
		.fragment(app::loadAsset(fragment));
	mUpdateShader = gl::GlslProg::create(shaderFmt);
	mUpdateShader->uniform("i_resolution", resolution);
}

void Feedback::update(const World& world)
{
	mUpdateShader->uniform("i_dt", world.deltaSource->delta());
	mUpdateShader->uniform("i_time", (float) app::getElapsedSeconds());

	world.audioSource->update();
	mTexture = world.audioSource->getMagSpectrumTexture();

	mUpdateShader->uniform("i_accumulatedSound", world.audioSource->getAccumulatedSound());
	mUpdateShader->uniform("i_highestVolume", world.audioSource->getHighestVolumePos());
}

void Feedback::draw(const World& world)
{
	gl::ScopedTextureBind audio(mTexture, 0);
	mUpdateShader->uniform("tex_audio", 0);

	gl::ScopedGlslProg glsl(mUpdateShader);
	gl::setMatricesWindow(world.windowSize);
	gl::drawSolidRect(world.bounds);
}

void Feedback::switchParams(OscVisController &controller)
{
	controller.subscribeSliderGlslListener("Volume", 0, 2, 1, mUpdateShader, "i_volume");
}
