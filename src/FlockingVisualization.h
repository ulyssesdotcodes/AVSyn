#pragma once

#include "cinder/gl/BufferTexture.h"

#include "AudioSource.h"
#include "BeatDetector.h"
#include "DeltaSource.h"
#include "Visualization.h"

/*
  A flocking simulation that responds to music.
*/
class FlockingVisualization : public Visualization {
public:
	FlockingVisualization();
	//! Update the position and velocity of each vertex. Also pass in the current values as textures to allow cross-vertex interaction.
	virtual void update(const World& world) override;
	//! Render the particles based on the positions calculated in update.
	virtual void draw(const World& world) override;
	//! So many parameters!
	virtual void switchParams(OscVisController &controller) override;

private:
	uint32_t mIteratonIndex;
	bool mStep;

	ci::gl::GlslProgRef mUpdateShader;
	ci::gl::GlslProgRef mRenderShader;

	std::array<ci::gl::VaoRef, 2> mVaos;
	std::array<ci::gl::VboRef, 2> mPositions, mVelocities, mColors;
	std::array<ci::gl::BufferTextureRef, 2> mPositionBufTex;
	std::array<ci::gl::BufferTextureRef, 2> mVelocityBufTex;

	float mAccumulatedLoudness;
	float mBeatConstant;
	float mLoudness;
	bool mSeparateOnly;
};