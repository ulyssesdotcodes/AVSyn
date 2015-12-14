#pragma once

#include "cinder/gl/gl.h"

#include "OscController.h"

class OscVisController {
public:
	OscVisController(const std::string address, std::shared_ptr<OscController> controller, std::vector<std::string> visualizationNames);
	void clear();
	void subscribeVisListener(std::function<void(int)>);
	void subscribeSliderListener(const std::string name, float min, float max, std::function<void(float)> observer);
	void subscribeSliderGlslListener(const std::string name, float min, float max, float defVal, ci::gl::GlslProgRef shader, const std::string uniformName);

private:
	const std::string mAddress;
	std::shared_ptr<OscController> mController;
	std::vector<Subscription> mSliderSubscriptions;
};