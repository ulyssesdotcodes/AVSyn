#pragma once

#include "OscListener.h"
#include "OscSender.h"

#include <map>

class OscController {
public:
	OscController();

	// Function that can be passed by value to
	void subscribe(const std::string &address, std::function<void(const ci::osc::Message)>);

	// Pass by value
	void sendMessage(ci::osc::Message message);

private:
	ci::osc::Listener mListener;
	ci::osc::Sender mSender;

	std::map<std::string, std::function<void(const ci::osc::Message)>> mObservers;
};