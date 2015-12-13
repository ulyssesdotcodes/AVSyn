#include "OscVisController.h"

using namespace ci;

OscVisController::OscVisController(const std::string &path, std::shared_ptr<OscController> controller, 
	std::vector<std::string> visualizationNames) : mPath(path), mController(controller)
{
}

void OscVisController::subscribeVisListener(std::function<void(int)> observer)
{
	mController->subscribe(mPath + "/choice", [observer](const osc::Message message) {
		if(message.getNumArgs() == 1) {
			observer((int) message.getArgAsFloat(0));
		}
	});
}


