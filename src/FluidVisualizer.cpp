#include "FluidVisualizer.h"

#include "cinder/app/App.h"
#include "cinder/Rand.h"

using namespace ci;

const int VELOCITY_POINTER = 0;
const int PRESSURE_POINTER = 1;
const int SMOKE_POINTER = 2;
const int ADVECT_POINTER = 3;

FluidVisualizer::FluidVisualizer()
{
	mLastTime = 0;
	mSmokePos = vec2(0.2, 0.8);
	mAudioVel = vec2(0);
	mAudioVelMult = vec2(0.05);
	mSpeed = 1.0;
	mFlipVelocity = false;

	//Setup shaders
	mWindowResolution = vec2(app::getWindowIndex(0)->getWidth(), app::getWindowIndex(0)->getHeight());
	mFluidResolution = glm::floor(mWindowResolution * vec2(0.2));

	mFluid = Fluid(mFluidResolution);

	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	updateFormat.fragment(app::loadAsset("Fluid/smoke_drop_forces.frag"));
	mForcesShader = gl::GlslProg::create(updateFormat);
	mForcesShader->uniform("i_resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/smoke_drop.frag"));
	mSmokeDropShader = gl::GlslProg::create(updateFormat);
	mSmokeDropShader->uniform("i_resolution", mWindowResolution);
	mSmokeDropShader->uniform("smokeDropPos", vec2(0.5, 0.8));

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

void FluidVisualizer::update(const World& world)
{
	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	world.audioSource->update();
	world.beatDetector->update(world, 1.6);

	if (dt > 1.0) {
		dt = 1.0;
	}

	updateSmokePos(world, time, dt);

	mForcesShader->uniform("i_dt", dt);

	// Update the fluid with the smoker's forces shader
	mFluid.update(dt, mForcesShader, mSmokeField.getTexture());

	// Drop new smoke
	gl::ScopedTextureBind scopeSmokeDrop(mSmokeField.getTexture(), 0);
	mSmokeDropShader->uniform("tex_prev", 0);
	mSmokeField.render(mSmokeDropShader);

	// Use the fluid to advect the smoke
	mFluid.advect(dt, &mSmokeField);
}

void FluidVisualizer::draw(const World& world)
{
	vec2 smokeSize = mSmokeField.getTexture()->getSize();
	gl::clear(Color(0, 0, 0));

	gl::ScopedTextureBind tex(mSmokeField.getTexture(), 2);
	mRenderShader->uniform("tex", 2);

	gl::ScopedViewport vp(ivec2(0), smokeSize);
	gl::ScopedMatrices scpM();
	gl::setMatricesWindow(smokeSize);
	gl::ScopedGlslProg glsl(mRenderShader);

	gl::drawSolidRect(Rectf(0, 0, smokeSize.x, smokeSize.y));
}

void FluidVisualizer::switchParams(OscVisController &controller)
{
	controller.subscribeSliderGlslListener("Volume", 0.5, 4.0, 2.0, mSmokeDropShader, "i_volume");
	controller.subscribeSliderListener("Speed", 0.5, 4.0, [&](float val) { mSpeed = val; });
}

void FluidVisualizer::updateSmokePos(const World& world, float time, float dt) {
	std::vector<float> audiovel = world.audioSource->getEqs(4);
	vec2 newVel = vec2(pow(audiovel[0], 1.5) - pow(audiovel[2], 1), pow(audiovel[1], 1.2) - pow(audiovel[3], 0.8));

	if (mFlipVelocity) {
		newVel = vec2(newVel.y, newVel.x);
	}

	mAudioVel = newVel * mSpeed  * mAudioVelMult;

	vec2 smokeDropPos = mSmokePos + mAudioVel;

	if (smokeDropPos.x < 0.1 && mAudioVel.x < 0) {
		mAudioVelMult.x = -mAudioVelMult.x;
	}
	else if (smokeDropPos.x > 0.9 && mAudioVel.x > 0) {
		mAudioVelMult.x = -mAudioVelMult.x;
	}

	if (smokeDropPos.y < 0.3 && mAudioVel.y < 0) {
		mAudioVelMult.y = -mAudioVelMult.y;
	}
	else if (smokeDropPos.y > 0.92 && mAudioVel.y > 0) {
		mAudioVelMult.y = -mAudioVelMult.y;
	}

	mSmokeDropShader->uniform("i_dt", dt);
	mSmokeDropShader->uniform("i_volume", world.audioSource->getVolume());
	mSmokeDropShader->uniform("i_beat", world.beatDetector->getBeat());
	mSmokeDropShader->uniform("i_smokeDropPos", smokeDropPos);
	mForcesShader->uniform("i_dt", dt);
	mForcesShader->uniform("i_time", (float) app::getElapsedSeconds());

	mSmokePos = smokeDropPos;
}