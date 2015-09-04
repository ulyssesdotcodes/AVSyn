#include "Fluid.h"
#include "cinder\app\App.h"
#include "cinder\Rand.h"

const int VELOCITY_POINTER = 0;
const int PRESSURE_POINTER = 1;
const int SMOKE_POINTER = 2;

void Fluid::setup(AudioSource *audioSource, BeatDetector *beatDetector)
{
	mAudioSource = audioSource;
	mBeatDetector = beatDetector;
	mWindowResolution = vec2(app::getWindowIndex(0)->getWidth(), app::getWindowIndex(0)->getHeight());
	mFluidResolution = glm::floor(mWindowResolution * vec2(0.2));

	//Setup shaders
	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	updateFormat.fragment(app::loadAsset("Fluid/advect.frag"));
	mAdvectShader = gl::GlslProg::create(updateFormat);
	mAdvectShader->uniform("resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/smoke_drop_forces.frag"));
	mForcesShader = gl::GlslProg::create(updateFormat);
	mForcesShader->uniform("resolution", mFluidResolution);
	mForcesShader->uniform("smokeDropPos", vec2(0.5, 0.8));

	updateFormat.fragment(app::loadAsset("Fluid/smoke_drop.frag"));
	mSmokeDropShader = gl::GlslProg::create(updateFormat);
	mSmokeDropShader->uniform("resolution", mWindowResolution);
	mSmokeDropShader->uniform("smokeDropPos", vec2(0.5, 0.8));

	updateFormat.fragment(app::loadAsset("Fluid/subtract_pressure.frag"));
	mSubtractPressureShader = gl::GlslProg::create(updateFormat);
	mSubtractPressureShader->uniform("resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/velocity_divergence.frag"));
	mDivergenceShader = gl::GlslProg::create(updateFormat);
	mDivergenceShader->uniform("resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/solve_pressure.frag"));
	mPressureSolveShader = gl::GlslProg::create(updateFormat);
	mPressureSolveShader->uniform("resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/render.frag"));
	mRenderShader = gl::GlslProg::create(updateFormat);
	mRenderShader->uniform("resolution", mWindowResolution);

	gl::Texture2d::Format texFmt;
	texFmt.setInternalFormat(GL_RGBA16F);
	texFmt.setDataType(GL_FLOAT);
	texFmt.setTarget(GL_TEXTURE_2D);
	texFmt.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	gl::Fbo::Format fmt;
	fmt.disableDepth()
		.setColorTextureFormat(texFmt);
	mVelocityFBO = PingPongFBO(fmt, mFluidResolution, 2);
	mPressureFBO = PingPongFBO(fmt, mFluidResolution, 2);
	mSmokeFBO = PingPongFBO(fmt, mWindowResolution, 2);
}

void Fluid::update()
{
	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	advect(dt);

	applyForce(dt);

	computeDivergence();
	solvePressure();
	subtractPressure();

	advectSmoke(dt, time);
}

void Fluid::draw()
{
	gl::ScopedTextureBind scopeSmoke(mSmokeFBO.getTexture(), SMOKE_POINTER);
	mRenderShader->uniform("tex_smoke", SMOKE_POINTER);

	gl::ScopedGlslProg glsl(mRenderShader);
	gl::context()->setDefaultShaderVars();

	gl::drawSolidRect(app::getWindowBounds());
}

bool Fluid::perspective()
{
	return false;
}

void Fluid::switchCamera(CameraPersp * camera)
{
}

void Fluid::switchParams(params::InterfaceGlRef params)
{
}

void Fluid::advect(float dt)
{
	mAdvectShader->uniform("dt", dt);
	mAdvectShader->uniform("boundaryConditions", true);
	mAdvectShader->uniform("target_resolution", mFluidResolution);

	gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
	mAdvectShader->uniform("tex_velocity", VELOCITY_POINTER);
	mAdvectShader->uniform("tex_target", VELOCITY_POINTER);

	gl::ScopedTextureBind scopePressure(mPressureFBO.getTexture(), PRESSURE_POINTER);
	mAdvectShader->uniform("tex_pressure", PRESSURE_POINTER);

	mVelocityFBO.render(mAdvectShader);
}

void Fluid::advectSmoke(float dt, float time) 
{

	// Advect the smoke
	mAdvectShader->uniform("target_resolution", mWindowResolution);
	mAdvectShader->uniform("dt", dt);
	mAdvectShader->uniform("boundaryConditions", false);

	gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
	mAdvectShader->uniform("tex_velocity", VELOCITY_POINTER);

	gl::ScopedTextureBind scopeSmoke(mSmokeFBO.getTexture(), SMOKE_POINTER);
	mAdvectShader->uniform("tex_target", SMOKE_POINTER);

	mSmokeFBO.render(mAdvectShader);

	// Create new smoke
	mAudioSource->update();
	mBeatDetector->update(1.6);
	mSmokeDropShader->uniform("beat", mBeatDetector->getBeat());
	mSmokeDropShader->uniform("volume", mAudioSource->getVolume());
	mSmokeDropShader->uniform("dt", dt);

	gl::ScopedTextureBind scopeSmokeDrop(mSmokeFBO.getTexture(), SMOKE_POINTER);
	mSmokeDropShader->uniform("tex_prev", SMOKE_POINTER);

	mSmokeFBO.render(mSmokeDropShader);
}

void Fluid::applyForce(float dt)
{
	mForcesShader->uniform("dt", dt);
	mForcesShader->uniform("time", mLastTime);

	gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
	mForcesShader->uniform("tex_velocity", VELOCITY_POINTER);

	mVelocityFBO.render(mForcesShader);
}

void Fluid::computeDivergence()
{
	gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
	mDivergenceShader->uniform("tex_velocity", VELOCITY_POINTER);
	gl::ScopedTextureBind scopePressure(mPressureFBO.getTexture(), PRESSURE_POINTER);
	mDivergenceShader->uniform("tex_pressure", PRESSURE_POINTER);

	mPressureFBO.render(mDivergenceShader);
}

void Fluid::solvePressure()
{
	for (int i = 0; i < 40; ++i) {
		gl::ScopedTextureBind scopePressure(mPressureFBO.getTexture(), PRESSURE_POINTER);
		mPressureSolveShader->uniform("tex_pressure", PRESSURE_POINTER);
		mPressureFBO.render(mPressureSolveShader);
	}
}

void Fluid::subtractPressure()
{
	gl::ScopedTextureBind scopeVel(mVelocityFBO.getTexture(), VELOCITY_POINTER);
	mSubtractPressureShader->uniform("tex_velocity", VELOCITY_POINTER);
	gl::ScopedTextureBind scopePressure(mPressureFBO.getTexture(), PRESSURE_POINTER);
	mSubtractPressureShader->uniform("tex_pressure", PRESSURE_POINTER);

	mVelocityFBO.render(mSubtractPressureShader);
}