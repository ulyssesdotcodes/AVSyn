#pragma once

#include "cinder\gl\gl.h"
#include "Visualization.h"
#include "AudioSource.h"

struct EQ {
	vec3 pos;
	vec3 velocity;
};

class EQPointCloud : public Visualization {

public:
	virtual void setup(AudioSource audioSource);
	virtual void update();
	virtual void draw();
	virtual void switchCamera(CameraPersp cam);
	bool perspective() override;

private:
	vector<vec3> mParticles;
	gl::GlslProgRef mRenderProg;
	gl::VboRef		mParticleBuffer[1];
	gl::BatchRef mBatch;

	AudioSource mAudioSource;

	vector<EQ> mEqs;
	vec3 mEqVolumes;
};
