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

	gl::GlslProgRef 
		mAdvectShader, 
		mForcesShader, 
		mDivergenceShader, 
		mPressureSolveShader, 
		mSubtractPressureShader, 
		mRenderShader,
		mSmokeDropShader;

	void advect(float dt);
	void advectSmoke(float dt, float time);
	void applyForce(float dt);
	void computeDivergence();
	void solvePressure();
	void subtractPressure();

	vec2 mWindowResolution;
	vec2 mFluidResolution;
	float mLastTime;

	PingPongFBO mVelocityFBO, mPressureFBO, mSmokeFBO;
};