#pragma once

#include "cinder/gl/gl.h"
#include "cinder/gl/BufferTexture.h"
#include "cinder/app/App.h"

#include "AudioSource.h"
#include "BeatDetector.h"
#include "PingPongFBO.h"
#include "Visualization.h"

class FluidVisualizer : public Visualization {
public:
	FluidVisualizer();
	virtual void update(const World& world) override;
	virtual void draw(const World& world) override;
	virtual void switchParams(ci::params::InterfaceGlRef params, const std::string &group) override;

private:
	ci::vec2 mWindowResolution,
		 mFluidResolution;

	ci::gl::GlslProgRef 
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
	ci::vec2 mAudioVel,
		mSmokePos,
		mAudioVelMult;

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