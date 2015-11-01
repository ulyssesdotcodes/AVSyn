#include "ChoiceVisualization.h"

ChoiceVisualization::ChoiceVisualization(map<string, shared_ptr<Visualization>> visualizations)
{
	mVisualizations = visualizations;
	for (map<string, shared_ptr<Visualization>>::iterator it = mVisualizations.begin(); it != mVisualizations.end(); ++it) {
		mVisualizationNames.push_back(it->first);
	}

	mVisualizationIndex = 0;
	mVisualization = visualizations[mVisualizationNames[mVisualizationIndex]];

	mFadeTransition = nullptr;
}

void ChoiceVisualization::update(const World& world)
{
	// If there's a transition, defer to it.
	if (mFadeTransition != nullptr) {
		mFadeTransition->update(world);
	}
	else {
		mVisualization->update(world);
	}
}

void ChoiceVisualization::draw(const World& world)
{
	// If there's a transition, defer to it
	// TODO: Use types
	if (mFadeTransition != nullptr) {
		mFadeTransition->draw(world);
		if (mFadeTransition->isFinished()) {
			mFadeTransition = nullptr;
		}
	}
	else{
		mVisualization->draw(world);
	}
}

void ChoiceVisualization::switchParams(ci::params::InterfaceGlRef params, const string & group)
{
	params->addParam("Visualization", mVisualizationNames, 
		[=](int ind) {
			VisualizationRef oldVisualization = shared_ptr<Visualization>(mVisualization);
			oldVisualization->resetParams(params);

			mVisualizationIndex = ind;
			mVisualization = mVisualizations[mVisualizationNames[mVisualizationIndex]];
			mVisualization->switchParams(params, group + "/Vis");

			if (mFade) {
				mFadeTransition = make_unique<FadeTransition>(oldVisualization, mVisualization, 5.0);
			}
		},
		[=]() {
			return mVisualizationIndex;
		}
	)
		.group(group);

	params->addParam("Fade", &mFade)
		.group(group);

	mVisualization->switchParams(params, group);
}
