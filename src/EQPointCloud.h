#pragma once

#include "cinder\gl\gl.h"
#include "Visualization.h"
#include "AudioSource.h"

struct EQ {
	vec3 pos;
	vec3 velocity;
};

/*
	A cube of particles that respond to the music. The first three particles are moved
	around the cube, each representing a third of the audio spectrum. They influence the
	color of the particles around them in a radius proportional to the volume of that 
	section of the audio spectrum.
*/
class EQPointCloud : public Visualization {

public:
	//! Create the particle buffer and shader program
	EQPointCloud();
	//! Update the three eq particles and the uniforms associated with eqs.
	virtual void update(const World& world);
	//! Draw the particles
	virtual void draw(const World& world);
	virtual void switchParams(params::InterfaceGlRef params, const string &group) override;

private:
	vector<vec3> mParticles;
	gl::GlslProgRef mRenderProg;
	gl::VboRef		mParticleBuffer[1];
	gl::BatchRef mBatch;

	vector<EQ> mEqs;
	vec3 mEqVolumes;

	float mLoudness;
	quat mRotation;
	float mRotationSpeed;
	float mHue;
	float mLastVolume;
};
