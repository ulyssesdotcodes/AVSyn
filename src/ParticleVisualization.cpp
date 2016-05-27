#include "ParticleVisualization.h"

using namespace ci;

ParticleVisualization::ParticleVisualization() : mStartPos(vec3(0.0f, 1.5f, 0.0f)), mStartVel(vec3(0))
{
}

void ParticleVisualization::update(const World & world)
{
	mAccumulatedSound += world.audioSource->getVolume();
	mParticleSystem.setTime(world.audioSource->getAccumulatedSound() * 128);

	float noise = mPerlin.fBm(vec3(mStartPos.x, mStartPos.y, app::getElapsedSeconds()));
	float angle = noise * 15.0f;
	mStartVel += vec3(cos(angle), sin(angle), 0) * world.deltaSource->delta();
	mStartVel = glm::normalize(mStartVel) * 0.02f;

	mStartPos += mStartVel;

	mParticleSystem.setStartVelocity(mStartVel);
	mParticleSystem.setStartPosition(mStartPos);

	if ((mStartPos.x > 1.5 && mStartVel.x > 0) || (mStartPos.x < -1.5 && mStartVel.x < 0)) {
		mStartVel.x = -mStartVel.x;
	}

	if ((mStartPos.y > 1.0 && mStartVel.y > 0) || (mStartPos.y < -1.0 && mStartVel.y < 0)) {
		mStartVel.y = -mStartVel.y;
	}


	mParticleSystem.update();
}

void ParticleVisualization::draw(const World & world)
{
	mParticleSystem.draw();
}

void ParticleVisualization::switchParams(OscVisController & controller)
{
	controller.subscribeSliderListener("Lifetime", 1, 128, [&](float val) { mParticleSystem.setLifetime(val); });
	mAccumulatedSound = 0;
}
