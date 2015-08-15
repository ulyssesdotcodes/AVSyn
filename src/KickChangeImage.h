#pragma once

#include "Visualization.h"
#include "AudioSource.h"
#include "BeatDetector.h"

struct Particle {
	vec3 pos;
	ColorA color;
	vec3 vel;
	vec3 home;
	float damping;
	vec2 ref;
};

class KickChangeImage : public Visualization {
public:
	virtual void setup(AudioSource* audioSource, BeatDetector* beatDetector);
	virtual void update();
	virtual void draw();
	virtual void switchCamera(CameraPersp* camera);
	virtual void switchParams(params::InterfaceGlRef params);
	virtual bool perspective() override;

private:
	AudioSource* mAudioSource;
	BeatDetector* mBeatDetector;
	float mLastBeat;

	vector<gl::TextureRef> mImages;
	int mCurrentImage;

	vector<Particle> mParticles;

	gl::GlslProgRef mRenderProg;
	gl::GlslProgRef mUpdateProg;

	gl::VaoRef mAttributes[2];
	gl::VboRef mParticleBuffer[2];
	int mIteration;

};