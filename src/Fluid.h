#pragma once

#include "Visualization.h"
#include "cinder\gl\gl.h"
#include "cinder\gl\BufferTexture.h"
#include "cinder\app\App.h"
#include "AudioSource.h"
#include "BeatDetector.h"
#include "PingPongFBO.h"

using namespace ci;

class Fluid : public Visualization {
public:
	virtual void setup(AudioSource *audioSource, BeatDetector *beatDetector);
	virtual void update() override;
	virtual void draw() override;
	virtual bool perspective() override;
	virtual void switchCamera(CameraPersp* camera) override;
	virtual void switchParams(params::InterfaceGlRef params) override;

private:
	AudioSource *mAudioSource;
	BeatDetector *mBeatDetector;

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
	void advectSmoke(float dt, float time);
	void advect(float dt, PingPongFBO *velocity, PingPongFBO *target);
	void applyForce(float dt);
	void computeDivergence();
	void solvePressure();
	void subtractPressure();
	void updateSmokePos(float time, float dt);
};