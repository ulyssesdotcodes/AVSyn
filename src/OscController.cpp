#include "OscController.h"

OscController::OscController()
{
	mListener.setup(3334);
}

void OscController::subscribe(const std::string &path, std::function<void(const ci::osc::Message*)> observer)
{
	mListener.registerMessageReceived([path, observer](const ci::osc::Message* message) {
		if (message->getAddress().compare(path) == 0) {
			observer(message);
		}
	});
}