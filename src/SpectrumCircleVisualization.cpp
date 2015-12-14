#include "SpectrumCircleVisualization.h"

SpectrumCircleVisualization::SpectrumCircleVisualization() : AudioShaderVisualization("circle.frag")
{
	mVolume = 1.0;
	mHoleSize = 0.2;
}

void SpectrumCircleVisualization::switchParams(OscVisController &controller)
{
	controller.subscribeSliderGlslListener("Volume", 0, 2, 1, mShader, "i_volume");
	controller.subscribeSliderGlslListener("Hole Size", 0, 1, 0.2, mShader, "i_holeSize");
}

void SpectrumCircleVisualization::renderUniforms(const World & world)
{
	mShader->uniform("i_holeSize", mHoleSize);
	mShader->uniform("i_volume", mVolume);
}
