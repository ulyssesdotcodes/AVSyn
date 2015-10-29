#pragma once

#include "cinder\app\App.h"

#include "Visualization.h"

using namespace ci;

class Fade
{
public:
	Fade(float transitionTime);
	void draw(Visualization* visA, Visualization* visB);

private:
	float mStartTime,
		mTransitionTime;

};