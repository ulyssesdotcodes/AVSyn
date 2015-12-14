#pragma once

#include "cinder/gl/gl.h"

#include "ChoiceVisualization.h"

class Mix {
public:
	Mix(const World& world, std::vector<std::string> orderedVisualizationNames, 
		std::map<std::string, std::shared_ptr<Visualization>> visualizations, std::shared_ptr<OscController> oscController);
	virtual void update(const World& world);
	virtual void draw(const World& world);

private:
	std::array<std::shared_ptr<ChoiceVisualization>, 2> mChoiceVises;
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

	void updateUniforms();
};