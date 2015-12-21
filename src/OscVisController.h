#pragma once

#include "BaseOscWrapper.h"

#include "cinder/gl/gl.h"

#include "OscController.h"

class OscVisController : BaseOscWrapper {
public:
	OscVisController(const std::string address, std::shared_ptr<OscController> controller, std::vector<std::string> visualizationNames);
	void clear() override;
	void clearSliders();
	void subscribeVisListener(std::function<void(int)>);
	void subscribeEffectListener(const std::string name, float min, float max, float defVal, ci::gl::GlslProgRef shader, const std::string uniformName);
	void subscribeEffectListener(const std::string name, float min, float max, float defVal, std::function<void(float)> observer);
	void subscribeEffectListener(const std::string name, bool defVal, std::function<void(bool)> observer);
	void subscribeSliderListener(const std::string name, float min, float max, std::function<void(float)> observer);
	void subscribeSliderGlslListener(const std::string name, float min, float max, float defVal, ci::gl::GlslProgRef shader, const std::string uniformName);

private:
	const std::string mAddress;
	std::vector<Subscription> mSliderSubscriptions;
};