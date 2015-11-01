#include "Feedback.h"
#include "cinder\app\App.h"

Feedback::Feedback(const string &fragment) : ShaderVisualization("texture.frag")
{
	mFade = 0.95;
	mLastTime = 0;

	vec2 resolution = app::getWindowIndex(0)->getSize();

	gl::Fbo::Format fmt;
	fmt.disableDepth();

	mFbo = PingPongFBO(fmt, resolution, 2);

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

	mUpdateShader->uniform("i_volume", world.audioSource->getVolume());
	mUpdateShader->uniform("i_accumulatedSound", world.audioSource->getAccumulatedSound());

	gl::ScopedTextureBind texPrev(mFbo.getTexture(), 0);
	mUpdateShader->uniform("tex_prev", 0);

	gl::ScopedTextureBind audio(mTexture, 1);
	mUpdateShader->uniform("tex_audio", 1);

	mUpdateShader->uniform("i_highestVolume", world.audioSource->getHighestVolumePos());
	mUpdateShader->uniform("i_fade", mFade);

	mFbo.render(mUpdateShader);
}

void Feedback::draw(const World& world)
{
	gl::ScopedTextureBind tex(mFbo.getTexture(), 0);
	mShader->uniform("tex", 0);

	ShaderVisualization::draw(world);
}

void Feedback::switchParams(params::InterfaceGlRef params, const string &group)
{
	addParamName(group + "/Fade");
	params->addParam(group + "/Fade", &mFade)
		.min(0.0)
		.max(1.0)
		.step(0.01)
		.group(group);
}
