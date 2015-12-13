#pragma once

#include "FadeTransition.h"
#include "OscVisController.h"
#include "Visualization.h"

class ChoiceVisualization : public Visualization
{
public:
	ChoiceVisualization(const World& world, std::vector<std::string> orderedVisualizationNames, 
		std::map<std::string, std::shared_ptr<Visualization>> visualizations, OscVisController *oscController);

	//! Perform all computations for the visualization.
	virtual void update(const World& world);
	//! Draw the visualization to the screen. This assumes matrices and viewport have been set up already.
	virtual void draw(const World& world);
	//! Set up the parameters for this visualization when the user switches to it.
	virtual void switchParams(ci::params::InterfaceGlRef params, const std::string &group);
	//! A convenience method so that parameters added in switchParams can be easily removed.
	virtual void setVisualization(int index);

private:
	std::map<std::string, std::shared_ptr<Visualization>> mVisualizations;
	std::vector<std::string> mVisualizationNames;
	std::shared_ptr<Visualization> mVisualization;
	int mVisualizationIndex;

	bool mFadeTransitionOn;
	std::unique_ptr<FadeTransition> mFadeTransition;

	ci::gl::FboRef mCurrentVis;
	PingPongFBO mPingPongFBO;

	ci::gl::GlslProgRef mFeedbackShader;

	float mFade,
		mOffsetY,
		mScale,
		mHueShift,
		mHueShiftCycle,
		mSaturationShift,
		mLightnessShift,
		mManipFade;

	ci::params::InterfaceGlRef mParams;
};