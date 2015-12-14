#include "NeuronVisualization.h"

#include "cinder/Rand.h"
#include "cinder/gl/gl.h"

#include "Neuron.h"

using namespace ci;

NeuronVisualization::NeuronVisualization(const World &world)
{
	mVolume = 1.0;

	int innerNeuronCount = OUTER_NEURON_COUNT - 4;

	auto randPos = [](vec2 area) { return vec2(Rand::randFloat() * area.x, Rand::randFloat() * area.y); };

	mInnerNeurons.push_back(std::make_shared<Neuron>(randPos(world.windowSize)));
	mInnerNeurons.push_back(std::make_shared<Neuron>(randPos(world.windowSize)));
	mInnerNeurons.push_back(std::make_shared<Neuron>(randPos(world.windowSize)));
	mInnerNeurons.push_back(std::make_shared<Neuron>(randPos(world.windowSize)));

	for (int i = 4; i < innerNeuronCount; ++i) {
		mInnerNeurons.push_back(std::make_shared<AxonNeuron>(randPos(world.windowSize), mInnerNeurons[(i >> 1) - 2]));
	}

	for (int i = 0; i < OUTER_NEURON_COUNT; ++i) {
		int diagIndex = i % 2 == 0 ? (OUTER_NEURON_COUNT / 2) + (i / 2) : (OUTER_NEURON_COUNT / 2) - (i / 2);
		float diagPos = (float)diagIndex / (float) OUTER_NEURON_COUNT;
		vec2 loc = (vec2) world.windowSize * vec2(diagPos);
		mOuterNeurons.push_back(std::make_shared<AxonNeuron>(loc, mInnerNeurons[((i + innerNeuronCount) >> 1) - 2]));
	}
	
}

void NeuronVisualization::update(const World & world)
{
	for (std::vector<std::shared_ptr<Neuron>>::iterator it = mInnerNeurons.begin(); it != mInnerNeurons.end(); ++it) {
		(*it)->update();
	}

	std::vector<float> volumes = world.audioSource->getEqs(OUTER_NEURON_COUNT);
	for (int i = 0; i < OUTER_NEURON_COUNT; ++i) {
		Impulse impulse;
		impulse.color = Colorf(ColorModel::CM_HSV, (float) i / (float) OUTER_NEURON_COUNT, 1.0, 1.0);
		impulse.intensity = volumes.at(i) * mVolume;

		mOuterNeurons.at(i)->synapse(impulse);
		mOuterNeurons.at(i)->update();
	}
}

void NeuronVisualization::draw(const World & world)
{
	for (std::vector<std::shared_ptr<Neuron>>::iterator it = mInnerNeurons.begin(); it != mInnerNeurons.end(); ++it) {
		(*it)->draw();
	}

	for (std::vector<std::shared_ptr<Neuron>>::iterator it = mOuterNeurons.begin(); it != mOuterNeurons.end(); ++it) {
		(*it)->draw();
	}
}

void NeuronVisualization::switchParams(OscVisController &controller)
{
	controller.subscribeSliderListener("Volume", 0, 2, [&](auto val) { mVolume = val;  });
}
