#pragma once

#include "OscListener.h"
#include "OscSender.h"

class OscController {
public:
	OscController();
	void subscribe(const std::string &path, std::function<void(const ci::osc::Message*)>);

private:
	ci::osc::Listener mListener;
	ci::osc::Sender mSender;
};