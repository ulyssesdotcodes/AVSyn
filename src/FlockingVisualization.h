#pragma once

#include "cinder\gl\BufferTexture.h"
#include "Visualization.h"
#include "AudioSource.h"
#include "DeltaSource.h"
#include "BeatDetector.h"

/*
  A flocking simulation that responds to music.
*/
class FlockingVisualization : public Visualization {
public:
	FlockingVisualization();
	//! Setup the various sources and buffers needed for the simulation
	void setup();
	//! Update the position and velocity of each vertex. Also pass in the current values as textures to allow cross-vertex interaction.
	virtual void update(const World& world) override;
	//! Render the particles based on the positions calculated in update.
	virtual void draw(const World& world) override;
	//! So many parameters!
	virtual void switchParams(params::InterfaceGlRef param, const string &group) override;

private:
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
	float mCycleHueSpeed;
	bool mSeparateOnly;
};