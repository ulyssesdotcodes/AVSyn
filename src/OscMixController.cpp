#include "OscMixController.h"

OscMixController::OscMixController(std::shared_ptr<OscController> controller) : BaseOscWrapper(controller)
{
}

void OscMixController::subscribeToConnection(std::function<void(const ci::osc::Message)> onConnection)
{
	ci::osc::Message message;
	message.setAddress("/connection");
	sendMessage(message);
	
	subscribe("/connection", onConnection, false);
}

void OscMixController::subscribeSliderListener(const std::string name, float min, float max, float defVal, std::function<void(float)> observer)
{
	subscribeFloatListener(getAddress("controls", name), name, min, max, defVal, observer);
}

void OscMixController::subscribeSliderListener(const std::string name, float min, float max, float defVal, ci::gl::GlslProgRef shader, const std::string uniformName)
{
	subscribeGlslListener(getAddress("controls", name), name, min, max, defVal, shader, uniformName);
}

std::string OscMixController::getAddress(const std::string path, const std::string name)
{
	std::ostringstream address;
	address << "/cinder/mix/" << path << "/" << name;
	return address.str();
}
