#pragma once

#include "Visualization.h"
#include "AudioSource.h"

struct Particle {
	vec3 pos;
	vec3 vel;
	vec3 home;
	ColorA color;
	float damping;
};

class KickChangeImage : public Visualization {
public:
	virtual void setup(AudioSource* audioSource);
	virtual void update();
	virtual void draw();
	virtual void switchCamera(CameraPersp* camera);
	virtual void switchParams(params::InterfaceGlRef params);
	virtual bool perspective() override;

private:
	AudioSource* mAudioSource;

	Surface32fRef mCurrentImage;

	vector<Particle> mParticles;

	gl::GlslProgRef mRenderProg;
	gl::GlslProgRef mUpdateProg;

	gl::VaoRef mAttributes[2];
	gl::VboRef mParticleBuffer[2];

	int mIteration;

};