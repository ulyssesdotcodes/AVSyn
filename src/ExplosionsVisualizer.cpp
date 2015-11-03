#include "ExplosionsVisualizer.h"

#include "cinder/Rand.h"

using namespace ci;
using namespace std;

const int VELOCITY_POINTER = 0;
const int PRESSURE_POINTER = 1;
const int SMOKE_POINTER = 2;
const int ADVECT_POINTER = 3;

ExplosionsVisualizer::ExplosionsVisualizer()
{
	mIntensity = 2.0;
	mFrequency = 0.07;

	//Setup shaders
	mWindowResolution = vec2(app::getWindowIndex(0)->getWidth(), app::getWindowIndex(0)->getHeight());
	mFluidResolution = glm::floor(mWindowResolution * vec2(0.2));

	mFluid = Fluid(mFluidResolution);

	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	updateFormat.fragment(app::loadAsset("Fluid/smoke_drop_forces.frag"));
	mForcesShader = gl::GlslProg::create(updateFormat);
	mForcesShader->uniform("i_resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/explosions.frag"));
	mSmokeDropShader = gl::GlslProg::create(updateFormat);
	mSmokeDropShader->uniform("i_resolution", mWindowResolution);

	updateFormat.fragment(app::loadAsset("Fluid/smoke_draw.frag"));
	mRenderShader = gl::GlslProg::create(updateFormat);
	mRenderShader->uniform("i_resolution", mWindowResolution);

	gl::Texture2d::Format texFmt;
	texFmt.setInternalFormat(GL_RGBA32F);
	texFmt.setDataType(GL_FLOAT);
	texFmt.setTarget(GL_TEXTURE_2D);
	texFmt.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	gl::Fbo::Format fmt;
	fmt.disableDepth()
		.setColorTextureFormat(texFmt);
	mSmokeField = PingPongFBO(fmt, mWindowResolution, 4);
}

void ExplosionsVisualizer::update(const World& world)
{
	float dt = world.deltaSource->delta();
	float time = app::getElapsedSeconds();

	if (dt > 1.0) {
		dt = 1.0;
	}

	updateSmokePos(world, time);

	// Update the fluid with the smoker's forces shader
	mFluid.update(dt, mForcesShader, mSmokeField.getTexture());

	// Drop new smoke
	gl::ScopedTextureBind scopeSmokeDrop(mSmokeField.getTexture(), 0);
	mSmokeDropShader->uniform("tex_prev", 0);
	mSmokeField.render(mSmokeDropShader);

	// Use the fluid to advect the smoke
	mFluid.advect(dt, &mSmokeField);
}

void ExplosionsVisualizer::draw(const World& world)
{
	vec2 smokeSize = mSmokeField.getTexture()->getSize();
	gl::clear(Color(0, 0, 0));

	gl::ScopedTextureBind tex(mSmokeField.getTexture(), 2);
	mRenderShader->uniform("tex", 2);

	gl::ScopedViewport vp(ivec2(0), smokeSize);
	gl::pushMatrices();
	gl::setMatricesWindow(smokeSize);
	gl::ScopedGlslProg glsl(mRenderShader);

	gl::drawSolidRect(Rectf(0, 0, smokeSize.x, smokeSize.y));

	gl::popMatrices();
}

void ExplosionsVisualizer::switchParams(params::InterfaceGlRef params, const std::string &group)
{
	addParamName(group + "/Explosions/Intensity");
	params->addParam(group + "/Explosions/Intensity", &mIntensity)
		.min(0.0f)
		.max(10.0f)
		.step(0.1f)
		.group(group);

	addParamName(group + "/Explosions/Frequency");
	params->addParam(group + "/Explosions/Frequency", &mFrequency)
		.min(0.0)
		.max(1.0f)
		.step(0.01f)
		.group(group);
}

void ExplosionsVisualizer::updateSmokePos(const World& world, float time) {
	if (Rand::randFloat() < mFrequency) {
		vec2 smokeDropPos = vec2(Rand::randFloat() * 0.8 + 0.1, Rand::randFloat() * 0.7 + 0.1);
		mSmokeDropShader->uniform("i_time", (float) app::getElapsedSeconds());
		mSmokeDropShader->uniform("i_dt", world.deltaSource->delta());
		mSmokeDropShader->uniform("i_smokeDropPos", smokeDropPos);
		mSmokeDropShader->uniform("i_intensity", mIntensity);
	}
	else {
		mSmokeDropShader->uniform("i_intensity", 0.0f);
	}

	mForcesShader->uniform("i_dt", world.deltaSource->delta());
	mForcesShader->uniform("i_time", (float) app::getElapsedSeconds());
}
