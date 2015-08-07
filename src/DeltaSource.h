#pragma once

class DeltaSource {
public:
	DeltaSource();
	void update();
	float delta();

private:
	float mLastTime;
	float mDelta;
};
