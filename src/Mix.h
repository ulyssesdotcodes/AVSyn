#pragma once

#include "cinder/gl/gl.h"

#include "ChoiceVisualization.h"
#include "OscMixController.h"

class Mix {
public:
	Mix(const World& world, std::vector<std::string> orderedVisualizationNames, 
		std::map<std::string, std::shared_ptr<Visualization>> visualizations, std::shared_ptr<OscController> oscController);
	virtual void update(const World& world);
	virtual void draw(const World& world);
	virtual void onConnection();

private:
	OscMixController mOscController;
	std::array<std::shared_ptr<ChoiceVisualization>, 2> mChoiceVises;
	std::array<ci::gl::FboRef, 2> mVisFBO;

	ci::gl::GlslProgRef mMixShader;

	float mFade;

	ci::CameraPersp* mCam;
	ci::params::InterfaceGlRef mParams;
};