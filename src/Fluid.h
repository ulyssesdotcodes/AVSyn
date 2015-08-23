#pragma once

#include "Visualization.h"

using namespace ci;

class Fluuid : public Visualization {
public:
	virtual void update() override;
	virtual void draw() override;
	virtual bool perspective() override;
	virtual void switchCamera(CameraPersp* camera) override;
	virtual void switchParams(params::InterfaceGlRef params) override;

private:

};