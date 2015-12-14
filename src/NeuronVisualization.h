#pragma once

#include "AxonNeuron.h"
#include "Neuron.h"
#include "Visualization.h"

const int OUTER_NEURON_COUNT = 64;

class NeuronVisualization : public Visualization {
public:
	NeuronVisualization(const World &world);

	virtual void update(const World &world) override;
	virtual void draw(const World &world) override;
	virtual void switchParams(OscVisController &controller) override;

private:
	float mVolume;

	std::vector<std::shared_ptr<Neuron>> mInnerNeurons;
	std::vector<std::shared_ptr<Neuron>> mOuterNeurons;
};