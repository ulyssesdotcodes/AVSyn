#include "SpectrumCircleVisualization.h"

SpectrumCircleVisualization::SpectrumCircleVisualization() : AudioShaderVisualization("circle.frag")
{
	mVolume = 1.0;
	mHoleSize = 0.2;
}

void SpectrumCircleVisualization::switchParams(ci::params::InterfaceGlRef params, const std::string & group)
{
	addParamName(group + "/Volume");
	params->addParam(group + "/Volume", &mVolume)
		.min(0.0)
		.max(2.0)
		.step(0.01)
		.group(group);

	addParamName(group + "/HoleSize");
	params->addParam(group + "/HoleSize", &mHoleSize)
		.min(0.0)
		.max(1.0)
		.step(0.01)
		.group(group);

}

void SpectrumCircleVisualization::renderUniforms(const World & world)
{
	mShader->uniform("i_holeSize", mHoleSize);
	mShader->uniform("i_volume", mVolume);
}
