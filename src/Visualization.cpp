#include "Visualization.h"

void Visualization::addParamName(const std::string &name) 
{
	mParams.push_back(name);
}

void Visualization::resetParams(ci::params::InterfaceGlRef params) 
{
	while (mParams.size() > 0) {
		params->removeParam(mParams.back());
		mParams.pop_back();
	}
}