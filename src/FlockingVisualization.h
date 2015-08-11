#pragma once

#include "cinder\gl\BufferTexture.h"
#include "Visualization.h"
#include "AudioSource.h"
#include "DeltaSource.h"
#include "BeatDetector.h"

class FlockingVisualization : public Visualization {
public:
	FlockingVisualization();
	void setup(AudioSource* audioSource, DeltaSource* deltaSource, BeatDetector* beatDetector);
	virtual void update() override;
	virtual void draw() override;
	virtual bool perspective() override;
	virtual void switchCamera(CameraPersp camera) override;
	virtual void switchParams(params::InterfaceGlRef params) override;

private:
	AudioSource* mAudioSource;
	DeltaSource* mDeltaSource;
	BeatDetector* mBeatDetector;

	uint32_t mIteratonIndex;
	bool mStep;

	gl::GlslProgRef mUpdateShader;
	gl::GlslProgRef mRenderShader;

	array<gl::VaoRef, 2> mVaos;
	array<gl::VboRef, 2> mPositions, mVelocities, mColors;
	array<gl::BufferTextureRef, 2> mPositionBufTex;
	array<gl::BufferTextureRef, 2> mVelocityBufTex;

	float mSeparationDistance;
	float mAlignmentDistance;
	float mCohesionDistance;
	float mRoamingDistance;
	float mAccumulatedLoudness;
	float mSpeed;
	float mBeatConstant;
	float mLoudness;
	float mHue;
	float mSaturation;
};