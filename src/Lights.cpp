#include "Lights.h"

#include "cinder\Rand.h"
#include "cinder\app\App.h"

void Lights::setup(AudioSource* audioSource)
{
	ShaderVisualization::setup("texture.frag");

	mAudioSource = audioSource;
	mFade = 0.9;

	mResolution = app::getWindowIndex(0)->getSize();

	gl::Fbo::Format fmt;
	fmt.disableDepth();

	mFbo = PingPongFBO(fmt, mResolution, 2);

	gl::GlslProg::Format shaderFmt;
	shaderFmt.vertex(app::loadAsset("passthru.vert"))
		.fragment(app::loadAsset("fade.frag"));
	mFadeShader = gl::GlslProg::create(shaderFmt);
	mFadeShader->uniform("i_resolution", mResolution);

	gl::GlslProgRef newLightShader = gl::getStockShader(gl::ShaderDef().color());
	mNewLight = gl::Batch::create(geom::Circle().radius(100.0), newLightShader);
}

void Lights::update()
{
	mFadeShader->uniform("i_fade", mFade);

	gl::ScopedTextureBind scopeTarget(mFbo.getTexture(), 0);
	mFadeShader->uniform("tex_prev", 0);

	mFbo.render(mFadeShader);

	float volume = mAudioSource->getVolume();
	float rand = Rand::randFloat();

	// Base the chance for a new light on the volume
	if (rand > volume * 0.5) {
		return;
	}

	float lightness = volume * 2.0;
	vec2 newLightLocation = vec2(Rand::randFloat() * mResolution.x, Rand::randFloat() * mResolution.y);
	mFbo.render(mNewLight, newLightLocation, Colorf(Rand::randFloat() * lightness, Rand::randFloat() * lightness, Rand::randFloat() * lightness));
	gl::popMatrices();
}

void Lights::draw()
{
	gl::ScopedTextureBind tex(mFbo.getTexture(), 0);
	mShader->uniform("tex", 0);

	ShaderVisualization::draw();
}

void Lights::switchParams(params::InterfaceGlRef params, const string & group)
{
}
