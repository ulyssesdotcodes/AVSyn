#pragma once

#include "Visualization.h"
#include "cinder\gl\gl.h"

using namespace ci;

class Mix : public Visualization {
public:
	void setup(map<string, Visualization*> visualizations, vector<string> visualizationOptions);
	virtual void update() override;
	virtual void draw() override;
	virtual bool perspective() override;
	virtual void switchCamera(ci::CameraPersp * cam) override;
	virtual void switchParams(ci::params::InterfaceGlRef params, const string &group) override;
	virtual void setBaseVisualization(const string &visualization);

private:
	map<string, Visualization*> mVisualizations;
	vector<string> mVisualizationOptions;
	array<int, 2> mVisOption;

	vec2 mResolution;
	array<gl::FboRef, 2> mVisFBO;

	gl::GlslProgRef 
		mMultiplyShader,
		mFadeShader;

	map<string, gl::GlslProgRef> mShaders;
	vector<string> mShaderOptions;
	int mShaderOption;

	float mFade;

	CameraPersp* mCam;
	params::InterfaceGlRef mParams;

	Visualization* getVis(int index);
};