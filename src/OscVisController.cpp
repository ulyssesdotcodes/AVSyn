#include "OscVisController.h"

#include <sstream>
#include "cinder/app/App.h"

using namespace ci;

OscVisController::OscVisController(const std::string address, std::shared_ptr<OscController> controller, 
	std::vector<std::string> visualizationNames) : BaseOscWrapper(controller), mAddress(address)
{
}

void OscVisController::clear()
{
	BaseOscWrapper::clear();
	mSliderSubscriptions.clear();
}

void OscVisController::clearSliders()
{
	std::ostringstream address;
	address << mAddress << "/sliders/clear";
	osc::Message message;
	message.setAddress(address.str());
	sendMessage(message);

	std::for_each(mSliderSubscriptions.begin(), mSliderSubscriptions.end(), [&](auto sub) { 
		sub.unsubscribe(); 
		removeSubscription(sub);
	});
	mSliderSubscriptions.clear();
}

void OscVisController::subscribeVisListener(std::function<void(int)> observer)
{
	subscribe(mAddress + "/choice", [observer](const osc::Message message) {
		if(message.getNumArgs() == 1) {
			observer((int) message.getArgAsInt32(0));
		}
	});
}

void OscVisController::subscribeEffectListener(const std::string name, float min, float max, float defVal, 
	gl::GlslProgRef shader, const std::string uniformName)
{
	std::ostringstream address;
	address << mAddress << "/effects/" << name;
	shader->uniform(uniformName, defVal);
	subscribeFloatListener(address.str(), name, min, max, defVal, [shader, uniformName](auto val) {
		shader->uniform(uniformName, val);
	});
}

void OscVisController::subscribeEffectListener(const std::string name, float min, float max, float defVal, std::function<void(float)> observer)
{
	std::ostringstream address;
	address << mAddress << "/effects/" << name;
	subscribeFloatListener(address.str(), name, min, max, defVal, observer);
}

void OscVisController::subscribeEffectListener(const std::string name, bool defVal, std::function<void(bool)> observer)
{
	std::ostringstream address;
	address << mAddress << "/effects/" << name;
	subscribeBoolListener(address.str(), name, defVal, observer);
}

void OscVisController::subscribeSliderListener(std::string name, float min, float max, std::function<void(float)> observer)
{
	std::ostringstream address;
	address << mAddress << "/sliders/" << mSliderSubscriptions.size();

	// Subscribe to the messages
	Subscription sub = subscribeFloatListener(address.str(), name, min, max, 0.5, observer);

	mSliderSubscriptions.push_back(sub);
}

void OscVisController::subscribeSliderGlslListener(const std::string name, float min, float max, float defVal, gl::GlslProgRef shader, const std::string uniformName)
{
	std::ostringstream address;
	address << mAddress << "/sliders/" << mSliderSubscriptions.size();

	// Subscribe to the messages
	Subscription sub = subscribeGlslListener(address.str(), name, min, max, defVal, shader, uniformName);

	mSliderSubscriptions.push_back(sub);
}
