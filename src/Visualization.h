#pragma once

#include "cinder\Camera.h"
#include "cinder\params\Params.h"

using namespace std;

class Visualization 
{
public:
	virtual void draw() = 0;
	virtual void update() = 0;
	virtual bool perspective() = 0;
	virtual void switchCamera(ci::CameraPersp cam) = 0;
	virtual void switchParams(ci::params::InterfaceGlRef params) = 0;
	virtual void addParamName(const string &param);
	virtual void resetParams(ci::params::InterfaceGlRef params);

private:
	vector<string> mParams;
};
