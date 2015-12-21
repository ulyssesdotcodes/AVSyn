#include "Neuron.h"

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

using namespace ci;

Neuron::Neuron(vec2 pos, gl::BatchRef circle) : mPos(pos), mCircleBatch(circle)
{
	mBuildingImpulse = Impulse();
	mBuildingImpulse.color = Colorf(0, 0, 0);
	mBuildingImpulse.intensity = 0.0f;
	mBuildingImpulseCount = 0;
}

void Neuron::update()
{
	if (mBuildingImpulse.intensity > THRESH) {
		mBuildingImpulse.time = app::getElapsedSeconds();
		mImpulses.push_back(mBuildingImpulse);
		mBuildingImpulse = Impulse();
		mBuildingImpulse.intensity = 0;
		mBuildingImpulseCount = 0;
	}

	for (std::vector<Impulse>::iterator it = mImpulses.begin(); it != mImpulses.end();) {
		it->intensity -= TRIGGERED_DEPLETION;
		if (deleteImpulse(*it)) {
			it = mImpulses.erase(it);
		}
		else {
			++it;
		}
	}

	mBuildingImpulse.intensity = glm::max(0.0f, mBuildingImpulse.intensity - GAINED_DEPLETION);
}

void Neuron::draw()
{
	Impulse highest;
	highest.intensity = 0;
	if (mImpulses.size() > 0) {
		for (std::vector<Impulse>::iterator it = mImpulses.begin(); it != mImpulses.end(); ++it) {
			highest = it->intensity > highest.intensity ? *it : highest;
		}
	}

	if (mBuildingImpulse.intensity > highest.intensity) {
		highest = mBuildingImpulse;
	}

	if (highest.intensity > 0) {
		gl::ScopedColor color(highest.color.lerp(1.0 - highest.intensity, Colorf(0, 0, 0)));
		gl::pushModelMatrix();
		gl::translate(mPos);
		gl::scale(vec2(8));
		mCircleBatch->draw();
		gl::popModelMatrix();
	}
}

void Neuron::synapse(Impulse impulse)
{
	mBuildingImpulseCount += 1;
	mBuildingImpulse.intensity = glm::min(impulse.intensity + mBuildingImpulse.intensity, 1.4f);
	mBuildingImpulse.color = mBuildingImpulse.color.lerp(1.0 / (float)mBuildingImpulseCount, impulse.color);
}

bool Neuron::deleteImpulse(Impulse impulse)
{
	return impulse.intensity < 0; 
}

std::vector<Impulse> Neuron::getImpulses()
{
	return mImpulses;
}
