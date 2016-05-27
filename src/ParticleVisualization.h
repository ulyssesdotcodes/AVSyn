#pragma once

#include "cinder/gl/BufferTexture.h"
#include "cinder/Perlin.h"

#include "Visualization.h"
#include "ParticleSystem.h"

/*
  A flocking simulation that responds to music.
*/
class ParticleVisualization : public Visualization {
public:
	ParticleVisualization();
	//! Update the position and velocity of each vertex. Also pass in the current values as textures to allow cross-vertex interaction.
	virtual void update(const World& world) override;
	//! Render the particles based on the positions calculated in update.
	virtual void draw(const World& world) override;
	virtual void switchParams(OscVisController &controller) override;

private:
	ci::Perlin mPerlin;
	ParticleSystem mParticleSystem;
	ci::vec3 mStartPos;
	ci::vec3 mStartVel;
	float mAccumulatedSound;
};
