#pragma once

#include "cinder\Camera.h"

class Visualization 
{
public:
	virtual void draw() = 0;
	virtual void update() = 0;
	virtual bool perspective() = 0;
	virtual void switchCamera(ci::CameraPersp cam) = 0;
	//virtual void setupParams() = 0;
};
