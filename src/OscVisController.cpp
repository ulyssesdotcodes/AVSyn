#include "OscVisController.h"

#include <sstream>
#include "cinder/gl/gl.h"

using namespace ci;

OscVisController::OscVisController(const std::string address, std::shared_ptr<OscController> controller, 
	std::vector<std::string> visualizationNames) : mAddress(address), mController(controller)
{
}

void OscVisController::clear()
{
	for (int i = 0; i < mSliderSubscriptions.size(); ++i){
		std::ostringstream address;
		address << mAddress << "/slider/" << i;
		osc::Message message;
		message.setAddress(address.str());
		message.addStringArg("--");
		mController->sendMessage(message);
	}

	std::for_each(mSliderSubscriptions.begin(), mSliderSubscriptions.end(), [](auto sub) { sub.unsubscribe(); });
	mSliderSubscriptions.clear();
}

void OscVisController::subscribeVisListener(std::function<void(int)> observer)
{
	mController->subscribe(mAddress + "/choice", [observer](const osc::Message message) {
		if(message.getNumArgs() == 1) {
			observer((int) message.getArgAsFloat(0));
		}
	});
}

void OscVisController::subscribeSliderListener(std::string name, float min, float max, std::function<void(float)> observer)
{
	std::ostringstream address;
	address << mAddress << "/slider/" << mSliderSubscriptions.size();

	// Send the name over
	osc::Message message;
	message.setAddress(address.str());
	message.addStringArg(name);
	mController->sendMessage(message);

	// Subscribe to the messages
	Subscription sub = mController->subscribe(address.str(), [min, max, observer](const osc::Message message) {
		if(message.getNumArgs() == 1) {
			observer(glm::mix(min, max, message.getArgAsFloat(0)));
		}
	});

	mSliderSubscriptions.push_back(sub);
}

void OscVisController::subscribeSliderGlslListener(const std::string name, float min, float max, float defVal, gl::GlslProgRef shader, const std::string uniformName)
{
	shader->uniform(uniformName, defVal);
	subscribeSliderListener(name, min, max, [shader, uniformName](auto val) {
		shader->uniform(uniformName, val);
	});
}
