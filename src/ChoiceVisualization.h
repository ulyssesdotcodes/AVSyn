#pragma once

#include "FadeTransition.h"
#include "Visualization.h"

class ChoiceVisualization : public Visualization
{
public:
	ChoiceVisualization(map<string, shared_ptr<Visualization>> visualizations);

	//! Perform all computations for the visualization.
	virtual void update(const World& world);
	//! Draw the visualization to the screen. This assumes matrices and viewport have been set up already.
	virtual void draw(const World& world);
	//! Set up the parameters for this visualization when the user switches to it.
	virtual void switchParams(ci::params::InterfaceGlRef params, const string &group);
	//! A convenience method so that parameters added in switchParams can be easily removed.


private:
	map<string, shared_ptr<Visualization>> mVisualizations;
	vector<string> mVisualizationNames;
	shared_ptr<Visualization> mVisualization;
	int mVisualizationIndex;

	bool mFade;
	unique_ptr<FadeTransition> mFadeTransition;
};