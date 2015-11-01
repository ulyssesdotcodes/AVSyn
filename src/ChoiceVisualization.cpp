#include "ChoiceVisualization.h"

ChoiceVisualization::ChoiceVisualization(map<string, shared_ptr<Visualization>> visualizations)
{
	mVisualizations = visualizations;
	for (map<string, shared_ptr<Visualization>>::iterator it = mVisualizations.begin(); it != mVisualizations.end(); ++it) {
		mVisualizationNames.push_back(it->first);
	}

	mVisualizationIndex = 0;
	mVisualization = visualizations[mVisualizationNames[mVisualizationIndex]];
}

void ChoiceVisualization::update(const World& world)
{
	mVisualization->update(world);
}

void ChoiceVisualization::draw(const World& world)
{
	mVisualization->draw(world);
}

void ChoiceVisualization::switchParams(ci::params::InterfaceGlRef params, const string & group)
{
	params->addSeparator(group);
	params->addParam(group + "/Visualization", mVisualizationNames, 
		[=](int ind) {
			mVisualization->resetParams(params);

			mVisualizationIndex = ind;
			mVisualization = mVisualizations[mVisualizationNames[mVisualizationIndex]];
			mVisualization->switchParams(params, group);
		},
		[=]() {
			return mVisualizationIndex;
		}
	);

	mVisualization->switchParams(params, group);
}
