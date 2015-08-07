#pragma once

#include "cinder\gl\BufferTexture.h"
#include "Visualization.h"
#include "AudioSource.h"
#include "DeltaSource.h"
#include "PingPongFBO.h"

class FlockingVisualization : public Visualization {
public:
	FlockingVisualization();
	void setup(AudioSource* audioSource, DeltaSource* deltaSource);
	virtual void update() override;
	virtual void draw() override;
	virtual bool perspective() override;
	virtual void switchCamera(CameraPersp camera) override;
	//void setupPingPongFbo();
	//void setupVbo();

private:
	AudioSource* mAudioSource;
	DeltaSource* mDeltaSource;

	uint32_t mIteratonIndex;
	bool mStep;

	gl::GlslProgRef mUpdateShader;
	gl::GlslProgRef mRenderShader;

	PingPongFBO mParticlesFbo;

	array<gl::VaoRef, 2> mVaos;
	array<gl::VboRef, 2> mPositions, mVelocities;
	array<gl::BufferTextureRef, 2> mPositionBufTex;
	array<gl::BufferTextureRef, 2> mVelocityBufTex;
	//gl::VboMeshRef mVboMesh;
	//gl::BatchRef mParticlesBatch;
	//gl::BatchRef mRenderBatch;

	float mSeparationDistance;
	float mAlignmentDistance;
	float mCohesionDistance;
	float mRoamingDistance;
	float mAccumulatedLoudness;
	float mSpeed;
};