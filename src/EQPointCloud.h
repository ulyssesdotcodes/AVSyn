#pragma once

#include "AudioShaderVisualization.h"

class EQPointCloud : public Visualization {

public:
	virtual void setup(AudioSource audioSource);
	virtual void update();
	virtual void draw();
	bool perspective() override;

private:
	gl::GlslProgRef mRenderProg;

	gl::VaoRef		mAttributes[1];
	gl::VboRef		mParticleBuffer[1];

	AudioSource mAudioSource;

	gl::BatchRef mBatch;
};
