#include "Feedback.h"
#include "cinder\app\App.h"

void Feedback::setup(AudioSource* audioSource, const string &fragment)
{
	ShaderVisualization::setup("texture.frag");

	mAudioSource = audioSource;

	mFade = 0.95;

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

void Feedback::update()
{
	mAudioSource->update();
	mTexture = mAudioSource->getMagSpectrumTexture();

	gl::ScopedTextureBind texPrev(mFbo.getTexture(), 0);
	mUpdateShader->uniform("tex_prev", 0);

	gl::ScopedTextureBind audio(mTexture, 1);
	mUpdateShader->uniform("tex_audio", 1);

	mUpdateShader->uniform("i_highestVolume", mAudioSource->getHighestVolumePos());
	mUpdateShader->uniform("i_fade", mFade);

	mFbo.render(mUpdateShader);
}

void Feedback::draw()
{
	gl::ScopedTextureBind tex(mFbo.getTexture(), 0);
	mShader->uniform("tex", 0);

	ShaderVisualization::draw();
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
