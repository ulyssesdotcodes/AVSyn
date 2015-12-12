#pragma once

#include "OscController.h"

class OscVisController {
public:
	OscVisController(const std::string &path, std::shared_ptr<OscController> controller);

	void subscribeVisListener(std::function<void(int)>);

private:
	const std::string &mPath;
	std::shared_ptr<OscController> mController;
};