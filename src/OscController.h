#pragma once

#include "Osc.h"
#include <map>

class Subscription {
public:
	Subscription(ci::osc::ReceiverUdp &listener, std::string address);
	Subscription& operator=(const Subscription&);
	void unsubscribe();
	bool operator==(const Subscription &other);

private:
	std::string mAddress;
	ci::osc::ReceiverUdp &mListener;
};

class OscController {
public:
	OscController();

	// Function that can be passed by value to
	Subscription subscribe(const std::string &address, std::function<void(const ci::osc::Message &message)>);

	// Pass by value
	void sendMessage(ci::osc::Message message);
	void sendBundle(ci::osc::Bundle bundle);

private:
	ci::osc::ReceiverUdp mListener;
	ci::osc::SenderUdp mSender;

	std::map<std::string, std::function<void(const ci::osc::Message)>> mObservers;
};