#pragma once

#include "OscListener.h"
#include "OscSender.h"

#include <map>

class Subscription {
public:
	Subscription(ci::CallbackId cbid, ci::osc::Listener &listener);
	void unsubscribe();

private:
	ci::osc::Listener &mListener;
	ci::CallbackId mCbid;
};

class OscController {
public:
	OscController();

	// Function that can be passed by value to
	Subscription subscribe(const std::string &address, std::function<void(const ci::osc::Message)>);

	// Pass by value
	void sendMessage(ci::osc::Message message);

private:
	ci::osc::Listener mListener;
	ci::osc::Sender mSender;

	std::map<std::string, std::function<void(const ci::osc::Message)>> mObservers;
};