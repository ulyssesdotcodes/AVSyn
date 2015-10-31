#pragma once

#include "Visualization.h"
#include "cinder\gl\gl.h"

using namespace ci;

class Mix : public Visualization {
public:
	void setup(map<string, shared_ptr<Visualization>> visualizations);
	virtual void update(const World& world) override;
	virtual void draw(const World& world) override;
	virtual void switchParams(ci::params::InterfaceGlRef params, const string &group) override;
	virtual void setBaseVisualization(const string &visualization);

private:
	map<string, shared_ptr<Visualization>> mVisualizations;
	vector<string> mVisualizationOptions;
	array<int, 2> mVisOption;

	vec2 mResolution;
	array<gl::FboRef, 2> mVisFBO;

	gl::GlslProgRef mMixShader;

	float mFade;
	float mAdd;
	float mMultiply;

	CameraPersp* mCam;
	params::InterfaceGlRef mParams;

	shared_ptr<Visualization> getVis(int index);

	void updateUniforms();
};