#pragma once

#include "Visualization.h"
#include "AudioSource.h"
#include "PingPongFBO.h"

class FlockingVisualization : public Visualization {
public:
	FlockingVisualization();
	void setup(AudioSource audioSource);
	virtual void update() override;
	virtual void draw() override;
	virtual bool perspective() override;
	virtual void switchCamera(CameraPersp camera) override;
	void setupPingPongFbo();
	void setupVbo();

private:
	AudioSource mAudioSource;

	bool mStep;

	gl::GlslProgRef mUpdateShader;
	gl::GlslProgRef mRenderShader;

	PingPongFBO mParticlesFbo;

	gl::VboMeshRef mVboMesh;
	gl::BatchRef mParticlesBatch;
	gl::BatchRef mRenderBatch;
};