#pragma once

#include "Observable.h"

template <class T>
class Observer {
public:
	virtual void update(T value) = 0;
};