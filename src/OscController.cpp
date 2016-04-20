#include "OscController.h"

#include "cinder/app/App.h"
#include "cinder/System.h"

using namespace ci;

OscController::OscController():mListener(3334), mSender(3333, System::getIpAddress(), 3333)
{
	mListener.bind();
	mListener.listen();
}

Subscription OscController::subscribe(const std::string &address, std::function<void(const osc::Message &message)> observer)
{
	mListener.setListener(address, [observer](const ci::osc::Message & msg) {
		app::console() << "Message Address" << msg.getAddress() << std::endl;
		const osc::Message copy = osc::Message(msg);
		app::AppBase::get()->dispatchAsync([observer, copy]() {
			observer(copy);
		});
	});;

	return Subscription(mListener, address);
}

void OscController::sendMessage(const osc::Message message) {
	mSender.send(message);
}

void OscController::sendBundle(const osc::Bundle bundle) {
	mSender.send(bundle);
}

Subscription::Subscription(ci::osc::ReceiverUdp &listener, std::string address) : mListener(listener), mAddress(address)
{
}

Subscription & Subscription::operator=(const Subscription &other)
{
	return Subscription(other.mListener, other.mAddress);
}

void Subscription::unsubscribe()
{
	mListener.removeListener(mAddress);
}

bool Subscription::operator==(const Subscription & other)
{
	return other.mAddress == mAddress;
}
