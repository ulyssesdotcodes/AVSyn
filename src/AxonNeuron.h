#pragma once

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"

#include "Neuron.h"

const float IMPULSE_TIME = 0.33f;
const int LINE_SEGMENTS = 8;

class AxonNeuron : public Neuron
{
public:
	AxonNeuron(const ci::vec2 &pos, std::shared_ptr<Neuron> neuron);

	virtual void update();
	virtual void draw();

protected:
	virtual bool deleteImpulse(Impulse impulse);

private:
	const std::shared_ptr<Neuron> mNeuron;
	ci::gl::VboMeshRef mAxonLine;
	std::vector<ci::ColorAf> mColors;
};