#pragma once

#include "cinder/gl/gl.h"
#include "cinder/gl/BufferTexture.h"
#include "cinder/app/App.h"

#include "AudioSource.h"
#include "BeatDetector.h"
#include "Fluid.h"
#include "PingPongFBO.h"
#include "Visualization.h"

class FluidVisualizer : public Visualization {
public:
	FluidVisualizer();
	virtual void update(const World& world) override;
	virtual void draw(const World& world) override;
	virtual void switchParams(ci::params::InterfaceGlRef params, const std::string &group) override;

private:
	Fluid mFluid;
	PingPongFBO mSmokeField;

	ci::vec2 mWindowResolution,
		 mFluidResolution;

	ci::gl::GlslProgRef 
		mRenderShader,
		mForcesShader, 
		mSmokeDropShader;

	float mLastTime;
	ci::vec2 mAudioVel,
		mSmokePos,
		mAudioVelMult;

	float mVolume;
	float mSpeed;
	float mSaturation;
	bool mFlipVelocity;

	void updateSmokePos(const World& world, float time, float dt);
};