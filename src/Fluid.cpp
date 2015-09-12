#include "Fluid.h"
#include "cinder\app\App.h"
#include "cinder\Rand.h"

const int VELOCITY_POINTER = 0;
const int PRESSURE_POINTER = 1;
const int SMOKE_POINTER = 2;
const int ADVECT_POINTER = 3;

void Fluid::setup(AudioSource *audioSource, BeatDetector *beatDetector)
{
	mAudioSource = audioSource;
	mBeatDetector = beatDetector;
	mLastTime = 0;
	mSmokePos = vec2(0.2, 0.8);
	mAudioVel = vec2(0);
	mAudioVelMult = vec2(0.05);
	mVolume = 2.0;
	mSpeed = 1.0;
	mFlipVelocity = false;
	mSaturation = 0.5;


	//Setup shaders
	mWindowResolution = vec2(app::getWindowIndex(0)->getWidth(), app::getWindowIndex(0)->getHeight());
	mFluidResolution = glm::floor(mWindowResolution * vec2(0.2));

	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	updateFormat.fragment(app::loadAsset("Fluid/advect.frag"));
	mAdvectShader = gl::GlslProg::create(updateFormat);
	mAdvectShader->uniform("resolution", mFluidResolution);

	updateFormat.fragment(app::loadAsset("Fluid/advect_maccormack.frag"));
	mAdvectMaccormackShader = gl::GlslProg::create(updateFormat);
	mAdvectMaccormackShader->uniform("resolution", mFluidResolution);

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

	updateFormat.fragment(app::loadAsset("texture.frag"));
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
	mVelocityFBO = PingPongFBO(fmt, mFluidResolution, 4);
	mSmokeFBO = PingPongFBO(fmt, mWindowResolution, 4);

	gl::Texture2d::Format texFmtRG;
	texFmtRG.setInternalFormat(GL_RG16F);
	texFmtRG.setDataType(GL_FLOAT);
	texFmtRG.setTarget(GL_TEXTURE_2D);
	texFmtRG.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	gl::Fbo::Format fmtRG;
	fmtRG.disableDepth()
		.setColorTextureFormat(texFmtRG);

	mPressureFBO = PingPongFBO(fmtRG, mFluidResolution, 2);
}

void Fluid::update()
{
	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	if (dt > 1.0) {
		dt = 1.0;
	}

	updateSmokePos(time, dt);


	advectVelocity(dt);

	applyForce(dt);

	computeDivergence();
	solvePressure();
	subtractPressure();

	advectSmoke(dt, time);
}

void Fluid::draw()
{
	gl::ScopedTextureBind scopeSmoke(mSmokeFBO.getTexture(), SMOKE_POINTER);
	mRenderShader->uniform("tex", SMOKE_POINTER);

	gl::ScopedGlslProg glsl(mRenderShader);
	gl::context()->setDefaultShaderVars();

	gl::drawSolidRect(app::getWindowIndex(0)->getBounds());
}

bool Fluid::perspective()
{
	return false;
}

void Fluid::switchCamera(CameraPersp * camera)
{
}

void Fluid::switchParams(params::InterfaceGlRef params, const string &group)
{
	addParamName(group + "/Volume");
	params->addParam(group + "/Volume", &mVolume)
		.min(0.5)
		.max(4.0f)
		.step(0.01f)
		.group(group);

	addParamName(group + "/Speed");
	params->addParam(group + "/Speed", &mSpeed)
		.min(0.5)
		.max(4.0)
		.step(0.01)
		.group(group);

	addParamName(group + "/Saturation");
	params->addParam(group + "/Saturation", &mSaturation)
		.min(0.0)
		.max(1.0)
		.step(0.01)
		.group(group);

	addParamName(group + "/Flip Velocity");
	params->addParam(group + "/Flip Velocity", &mFlipVelocity);
}

void Fluid::advectVelocity(float dt)
{
	mAdvectShader->uniform("boundaryConditions", true);
	mAdvectShader->uniform("target_resolution", mFluidResolution);

	mAdvectMaccormackShader->uniform("boundaryConditions", true);
	mAdvectMaccormackShader->uniform("target_resolution", mFluidResolution);

	advect(dt, &mVelocityFBO, &mVelocityFBO);
}

void Fluid::advectSmoke(float dt, float time) 
{
	// Create new smoke
	mAudioSource->update();
	mBeatDetector->update(1.6);
	{
		mSmokeDropShader->uniform("beat", mBeatDetector->getBeat());
		mSmokeDropShader->uniform("volume", mAudioSource->getVolume() * mVolume);
		mSmokeDropShader->uniform("dt", dt);

		gl::ScopedTextureBind scopeSmokeDrop(mSmokeFBO.getTexture(), SMOKE_POINTER);
		mSmokeDropShader->uniform("tex_prev", SMOKE_POINTER);

		mSmokeFBO.render(mSmokeDropShader);
	}

	mAdvectShader->uniform("boundaryConditions", false);
	mAdvectShader->uniform("target_resolution", mWindowResolution);

	mAdvectMaccormackShader->uniform("boundaryConditions", false);
	mAdvectMaccormackShader->uniform("target_resolution", mWindowResolution);

	advect(dt, &mVelocityFBO, &mSmokeFBO);
}

void Fluid::advect(float dt, PingPongFBO *velocity, PingPongFBO *target) {
	{
		mAdvectShader->uniform("dt", dt);
		gl::ScopedTextureBind scopeVel(velocity->getTexture(), VELOCITY_POINTER);
		mAdvectShader->uniform("tex_velocity", VELOCITY_POINTER);
		gl::ScopedTextureBind scopeTarget(target->getTexture(), ADVECT_POINTER);
		mAdvectShader->uniform("tex_target", ADVECT_POINTER);

		target->render(mAdvectShader);
	}

	// Run time backwards for the second one
	{
		mAdvectShader->uniform("dt", -dt);
		gl::ScopedTextureBind scopeVel(velocity->getTexture(), VELOCITY_POINTER);
		mAdvectShader->uniform("tex_velocity", VELOCITY_POINTER);
		gl::ScopedTextureBind scopeTarget(target->getTexture(), ADVECT_POINTER);
		mAdvectShader->uniform("tex_target", ADVECT_POINTER);

		target->render(mAdvectShader);
	}

	{
		mAdvectMaccormackShader->uniform("dt", dt);
		gl::ScopedTextureBind scopeVel(velocity->getTexture(), VELOCITY_POINTER);
		mAdvectMaccormackShader->uniform("tex_velocity", VELOCITY_POINTER);

		vector<gl::TextureRef> textures = target->getTextures();
		gl::ScopedTextureBind scopedPhiN(textures.at(1), 3);
		mAdvectMaccormackShader->uniform("phi_n", 3);
		mAdvectMaccormackShader->uniform("tex_target", 3);
		gl::ScopedTextureBind scopedPhiN1Hat(textures.at(2), 4);
		mAdvectMaccormackShader->uniform("phi_n_1_hat", 4);
		gl::ScopedTextureBind scopedPhiNHat(textures.at(3), 5);
		mAdvectMaccormackShader->uniform("phi_n_hat", 5);

		target->render(mAdvectMaccormackShader);
	}
}

void Fluid::applyForce(float dt)
{
	mBeatDetector->update(1.6);
	mForcesShader->uniform("dt", dt);
	mForcesShader->uniform("time", mLastTime);
	mForcesShader->uniform("beat", mBeatDetector->getBeat());

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

void Fluid::updateSmokePos(float time, float dt) {
	mAudioSource->update();
	vector<float> audiovel = mAudioSource->getEqs(4);
	vec2 newVel = vec2(pow(audiovel[0], 1.5) - pow(audiovel[2], 1), pow(audiovel[1], 1.2) - pow(audiovel[3], 0.8));
	newVel = newVel * vec2(0.5) + vec2(0.5) * newVel * mBeatDetector->getBeat();

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

	mSmokeDropShader->uniform("smokeDropPos", smokeDropPos);
	mSmokeDropShader->uniform("hue", cos(time * 0.5f));
	mSmokeDropShader->uniform("saturation", mSaturation);
	mForcesShader->uniform("smokeDropPos", smokeDropPos);
	mForcesShader->uniform("smokeVel", smokeDropPos - mSmokePos);

	mSmokePos = smokeDropPos;
}