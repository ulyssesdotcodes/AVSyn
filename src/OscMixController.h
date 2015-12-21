#pragma once

#include "BaseOscWrapper.h"

#include "cinder/gl/gl.h"

#include "OscController.h"

class OscMixController : BaseOscWrapper {
public:
	OscMixController(std::shared_ptr<OscController> controller);

	void subscribeToConnection(std::function<void(const ci::osc::Message)> onConnection);
	void subscribeSliderListener(const std::string name, float min, float max, float defVal, std::function<void(float)> observer);
	void subscribeSliderListener(const std::string name, float min, float max, float defVal, ci::gl::GlslProgRef shader, const std::string uniformName);

private:
	std::vector<Subscription> mSubscriptions;
	std::string getAddress(const std::string path, const std::string name);
};