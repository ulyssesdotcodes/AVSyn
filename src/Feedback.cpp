#include "Feedback.h"

#include "cinder/app/App.h"

using namespace ci;

Feedback::Feedback(const std::string &fragment)
{
	mLastTime = 0;
	mVolume = 1.0f;

	vec2 resolution = app::getWindowIndex(0)->getSize();

	gl::GlslProg::Format shaderFmt;
	shaderFmt.vertex(app::loadAsset("passthru.vert"))
		.fragment(app::loadAsset(fragment));
	mUpdateShader = gl::GlslProg::create(shaderFmt);
	mUpdateShader->uniform("i_resolution", resolution);
}

void Feedback::update(const World& world)
{
	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	mUpdateShader->uniform("i_dt", dt);
	mUpdateShader->uniform("i_time", time);

	world.audioSource->update();
	mTexture = world.audioSource->getMagSpectrumTexture();

	mUpdateShader->uniform("i_volume", world.audioSource->getVolume() * mVolume);
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

void Feedback::switchParams(params::InterfaceGlRef params, const std::string &group)
{
	addParamName(group + "/Volume");
	params->addParam(group + "/Volume", &mVolume)
		.min(0.0)
		.max(2.0)
		.step(0.02)
		.group(group);
}
