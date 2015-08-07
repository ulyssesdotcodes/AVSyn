#include "DeltaSource.h"

#include "cinder\app\App.h"

using namespace ci;

DeltaSource::DeltaSource() {
	mLastTime = app::getElapsedSeconds();
	mDelta = 0.0;
}

void DeltaSource::update() {
	float time = app::getElapsedSeconds();
	mDelta = time - mLastTime;
	mLastTime = time;
}

float DeltaSource::delta() {
	return mDelta;
}