#include "Fade.h"

Fade::Fade(float transitionTime)
{
	mTransitionTime = transitionTime;

	mStartTime = app::getElapsedSeconds();
}

void Fade::draw(Visualization * visA, Visualization * visB)
{
	float time = app::getElapsedSeconds();
	if (mStartTime + mTransitionTime < time) {
		// Transition finished
	}
	else {
		// Transition in progress
	}
}
