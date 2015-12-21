#pragma once

#include "cinder/gl/gl.h"
#include "boost/signals2.hpp"

const float THRESH = 0.5;
const float GAINED_DEPLETION = 0.1;
const float TRIGGERED_DEPLETION = 0.05;

struct Impulse {
	float intensity;
	float time;
	ci::Colorf color;
};

class Neuron 
{
public:
	ci::vec2 const mPos;

	Neuron(ci::vec2 pos, ci::gl::BatchRef circle);

	virtual void update();
	virtual void draw();
	virtual void synapse(Impulse impulse);

protected:
	std::vector<Impulse> getImpulses();

	virtual bool deleteImpulse(Impulse impulse);

private:
	ci::gl::BatchRef mCircleBatch;

	std::vector<Impulse> mImpulses;

	Impulse mBuildingImpulse;
	int mBuildingImpulseCount;

};