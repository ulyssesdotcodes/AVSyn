#pragma once

#include "Visualization.h"
#include "cinder\gl\gl.h"
#include "cinder\gl\BufferTexture.h"
#include "cinder\app\App.h"
#include "AudioSource.h"
#include "BeatDetector.h"
#include "PingPongFBO.h"

using namespace ci;

class FluidVisualizer : public Visualization {
public:
	FluidVisualizer();
	virtual void update(const World& world) override;
	virtual void draw(const World& world) override;
	virtual void switchParams(params::InterfaceGlRef params, const string &group) override;

private:
	vec2 mWindowResolution;
	vec2 mFluidResolution;

	gl::GlslProgRef 
		mAdvectShader, 
		mAdvectMaccormackShader,
		mForcesShader, 
		mDivergenceShader, 
		mPressureSolveShader, 
		mSubtractPressureShader, 
		mRenderShader,
		mSmokeDropShader;

	PingPongFBO mVelocityFBO, mPressureFBO, mSmokeFBO;

	float mLastTime;
	vec2 mAudioVel;
	vec2 mSmokePos;
	vec2 mAudioVelMult;

	float mVolume;
	float mSpeed;
	float mSaturation;
	bool mFlipVelocity;

	void advectVelocity(float dt);
	void advectSmoke(const World& world, float dt, float time);
	void advect(float dt, PingPongFBO *velocity, PingPongFBO *target);
	void applyForce(const World& world, float dt);
	void computeDivergence();
	void solvePressure();
	void subtractPressure();
	void updateSmokePos(const World& world, float time, float dt);
};