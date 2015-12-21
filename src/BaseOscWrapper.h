#pragma once

#include "OscController.h"

#include "cinder/gl/gl.h"

class BaseOscWrapper {
public:
	BaseOscWrapper(std::shared_ptr<OscController> controller);
	virtual void clear();

protected:
	void sendMessage(ci::osc::Message message);
	Subscription subscribe(const std::string &address, std::function<void(const ci::osc::Message)> observer, bool store = true);
	Subscription subscribeFloatListener(const std::string address, const std::string name, float min, float max, float defVal, std::function<void(float)> observer);
	Subscription subscribeBoolListener(const std::string address, const std::string name, bool defVal, std::function<void(bool)> observer);
	Subscription subscribeGlslListener(const std::string address,  std::string name, float min, float max, float defVal, ci::gl::GlslProgRef shader, const std::string uniformName);
	void removeSubscription(const Subscription sub);

private:
	std::shared_ptr<OscController> mController;
	std::vector<Subscription> mSubscriptions;
};