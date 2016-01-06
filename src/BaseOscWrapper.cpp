#include "BaseOscWrapper.h"

using namespace ci;

BaseOscWrapper::BaseOscWrapper(std::shared_ptr<OscController> controller) : mController(controller)
{
}

void BaseOscWrapper::clear()
{
	std::for_each(mSubscriptions.begin(), mSubscriptions.end(), [](auto sub) { sub.unsubscribe(); });
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
	message.addIntArg(32); // Flag for a slider message
	message.addStringArg(name);
	message.addIntArg(0);
	message.addFloatArg(min);
	message.addFloatArg(max);
	message.addFloatArg(defVal);
	mController->sendMessage(message);

	// Subscribe to the messages
	return subscribe(address + "/value", [min, max, observer](const osc::Message message) {
		if(message.getNumArgs() == 1) {
			observer(message.getArgAsFloat(0));
		}
	});
}

Subscription BaseOscWrapper::subscribeBoolListener(const std::string address, const std::string name, bool defVal, std::function<void(bool)> observer)
{
	// Send the name over
	osc::Message message;
	message.setAddress(address);
	message.addIntArg(64); // Flag for a bool message
	message.addStringArg(name);
	message.addIntArg(1);
	message.addIntArg(defVal ? 1 : 0);
	mController->sendMessage(message);
	
	return subscribe(address + "/value", [observer](const osc::Message message) {
		observer(message.getArgAsFloat(0) == 1);
	});
}

Subscription BaseOscWrapper::subscribeGlslListener(const std::string address, std::string name, float min, float max, float defVal, ci::gl::GlslProgRef shader, const std::string uniformName)
{
	shader->uniform(uniformName, defVal);
	return subscribeFloatListener(address, name, min, max, defVal, [shader, uniformName](auto val) {
		shader->uniform(uniformName, val);
	});
}

void BaseOscWrapper::removeSubscription(const Subscription sub)
{
	mSubscriptions.erase(std::remove(mSubscriptions.begin(), mSubscriptions.end(), sub), mSubscriptions.end());
}
