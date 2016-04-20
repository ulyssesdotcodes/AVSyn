#include "BaseOscWrapper.h"

using namespace ci;

BaseOscWrapper::BaseOscWrapper(std::shared_ptr<OscController> controller) : mController(controller)
{
}

void BaseOscWrapper::clear()
{
	std::for_each(mSubscriptions.begin(), mSubscriptions.end(), [](Subscription sub) { sub.unsubscribe(); });
	mSubscriptions.clear();
}

void BaseOscWrapper::sendMessage(ci::osc::Message message)
{
	mController->sendMessage(message);
}

Subscription BaseOscWrapper::subscribe(const std::string &address, std::function<void(const ci::osc::Message)> observer, bool store)
{
	Subscription sub = mController->subscribe(address, observer);
	if(store) {
		mSubscriptions.push_back(sub);
	}
	return sub;
}

Subscription BaseOscWrapper::subscribeFloatListener(const std::string address, const std::string name, float min, float max, float defVal, std::function<void(float)> observer)
{
	// Send the name over
	osc::Message message;
	message.setAddress(address);
	message.append(32); // Flag for a slider message
	message.append(name);
	message.append(0);
	message.append(min);
	message.append(max);
	message.append(defVal);
	mController->sendMessage(message);

	// Subscribe to the messages
	return subscribe(address + "/value", [min, max, observer](const osc::Message &message) {
		if(message.getNumArgs() == 1) {
			observer(message[0].flt());
		}
	});
}

Subscription BaseOscWrapper::subscribeBoolListener(const std::string address, const std::string name, bool defVal, std::function<void(bool)> observer)
{
	// Send the name over
	osc::Message message;
	message.setAddress(address);
	message.append(64); // Flag for a bool message
	message.append(name);
	message.append(1);
	message.append(defVal ? 1 : 0);
	mController->sendMessage(message);
	
	return subscribe(address + "/value", [observer](const osc::Message &message) {
		observer(message[0].flt() == 1);
	});
}

Subscription BaseOscWrapper::subscribeGlslListener(const std::string address, std::string name, float min, float max, float defVal, ci::gl::GlslProgRef shader, const std::string uniformName)
{
	shader->uniform(uniformName, defVal);
	return subscribeFloatListener(address, name, min, max, defVal, [shader, uniformName](float val) {
		shader->uniform(uniformName, val);
	});
}

void BaseOscWrapper::removeSubscription(const Subscription sub)
{
	mSubscriptions.erase(std::remove(mSubscriptions.begin(), mSubscriptions.end(), sub), mSubscriptions.end());
}
