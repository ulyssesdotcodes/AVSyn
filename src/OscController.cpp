#include "OscController.h"

#include "cinder/app/App.h"
#include "cinder/System.h"

using namespace ci;

OscController::OscController()
{
	mListener.setup(3334);
	mSender.setup(System::getIpAddress(), 3333, true);
}

Subscription OscController::subscribe(const std::string &address, std::function<void(const osc::Message)> observer)
{
	CallbackId cbid = 
		mListener.registerMessageReceived([address, observer](const osc::Message* message) {
			if (address.compare(message->getAddress()) == 0) {
				const osc::Message copy = osc::Message(*message);
				app::AppBase::get()->dispatchAsync([observer, copy]() {
					observer(copy);
				});
			}
		});

	return Subscription(cbid, mListener);
}

void OscController::sendMessage(const osc::Message message) {
	mSender.sendMessage(message);
}

Subscription::Subscription(ci::CallbackId cbid, ci::osc::Listener &listener) : mCbid(cbid), mListener(listener)
{
}

void Subscription::unsubscribe()
{
	mListener.unregisterMessageReceived(mCbid);
}
