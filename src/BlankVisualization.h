#pragma once

#include "Visualization.h"

class BlankVisualization : public Visualization {
public:
	//! Perform all computations for the visualization.
	virtual void update(const World& world);
	//! Draw the visualization to the screen. This assumes matrices and viewport have been set up already.
	virtual void draw(const World& world);
	//! Set up the parameters for this visualization when the user switches to it.
	virtual void switchParams(ci::params::InterfaceGlRef params, const std::string &group);
	//! A convenience method so that parameters added in switchParams can be easily removed.
};