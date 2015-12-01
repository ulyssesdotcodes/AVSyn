#include "AxonNeuron.h"

using namespace ci;

AxonNeuron::AxonNeuron(const ci::vec2 &pos, std::shared_ptr<Neuron> neuron) : 
	Neuron(pos), mNeuron(neuron)
{

	std::vector<vec3> positions;
	for (int i = 0; i < LINE_SEGMENTS; i++) {
		positions.push_back(vec3(glm::mix(pos, neuron->mPos, (float) i / (float) (LINE_SEGMENTS - 1)), 0));
		mColors.push_back(ColorAf(0, 0, 0, 0));
	}

	std::vector<gl::VboMesh::Layout> layout = {
		gl::VboMesh::Layout().usage(GL_STATIC_DRAW).attrib(geom::Attrib::POSITION, 3),
		gl::VboMesh::Layout().usage(GL_DYNAMIC_DRAW).attrib(geom::Attrib::COLOR, 4)
	};

	mAxonLine = gl::VboMesh::create(LINE_SEGMENTS, GL_LINE_STRIP, layout);

	mAxonLine->bufferAttrib(geom::Attrib::POSITION, positions);
	mAxonLine->bufferAttrib(geom::Attrib::COLOR, mColors);
}

void AxonNeuron::update() {
	Neuron::update();

	std::vector<Impulse> impulses = getImpulses();
	float time = app::getElapsedSeconds();

	mColors.assign(LINE_SEGMENTS, ColorAf(0, 0, 0, 0));

	for (int i = 0; i < impulses.size(); ++i) {
		Impulse impulse = impulses.at(i);
		if (abs(impulse.time + IMPULSE_TIME - time) < 0.167) {
			mNeuron->synapse(impulse);
		}

		float percentComplete = (app::getElapsedSeconds() - impulse.time) / IMPULSE_TIME;
		int vertex = (int) glm::round(percentComplete * LINE_SEGMENTS);

		for (int i = -2; i < 3; ++i) {
			int colorVert = vertex + i;
			Colorf color = impulse.color;
			if(colorVert < LINE_SEGMENTS && colorVert >= 0) {
				color = color.lerp((float)abs(colorVert - vertex) / 3.0f, Colorf(0, 0, 0));
				mColors.at(colorVert) = ColorAf(color.r, color.g, color.b, 1);
			}
		}
	}

	mAxonLine->bufferAttrib(geom::Attrib::COLOR, mColors);
}

void AxonNeuron::draw()
{
	Neuron::draw();

	gl::lineWidth(1.0f);
	gl::draw(mAxonLine);
}

bool AxonNeuron::deleteImpulse(Impulse impulse) {
	return Neuron::deleteImpulse(impulse) && impulse.time - IMPULSE_TIME > app::getElapsedSeconds();
}