#pragma once

#include "cinder/gl/gl.h"

#include "Visualization.h"

class Mix : public Visualization {
public:
	Mix(std::map<std::string, std::shared_ptr<Visualization>> visualizations);
	virtual void update(const World& world) override;
	virtual void draw(const World& world) override;
	virtual void switchParams(ci::params::InterfaceGlRef params, const std::string &group) override;
	virtual void setBaseVisualization(const std::string &visualization);

private:
	std::map<std::string, std::shared_ptr<Visualization>> mVisualizations;
	std::vector<std::string> mVisualizationOptions;
	std::array<int, 2> mVisOption;

	ci::vec2 mResolution;
	std::array<ci::gl::FboRef, 2> mVisFBO;

	ci::gl::GlslProgRef mMixShader;

	float mFade;
	float mAdd;
	float mMultiply;

	ci::CameraPersp* mCam;
	ci::params::InterfaceGlRef mParams;

	std::shared_ptr<Visualization> getVis(int index);

	void updateUniforms();
};