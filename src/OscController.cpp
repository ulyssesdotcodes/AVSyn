#include "OscController.h"

#include "cinder/app/App.h"
#include "cinder/System.h"

using namespace ci;

OscController::OscController()
{
	mListener.setup(3334);
	mSender.setup(System::getIpAddress(), 3333, true);
}

void OscController::subscribe(const std::string &address, std::function<void(const osc::Message)> observer)
{
	// By design, only one thing can be listening to a path at a time.
	mObservers[address] = observer;
	mListener.registerMessageReceived([address, observer](const osc::Message* message) {
		if (address.compare(message->getAddress()) == 0) {
			const osc::Message copy = osc::Message(*message);
			app::AppBase::get()->dispatchAsync([observer, copy]() {
				observer(copy);
			});
		}
	});
}

void OscController::sendMessage(const osc::Message message) {
	mSender.sendMessage(message);
}