/*
 * lock_guard.cpp
 *
 *  Created on: Apr 24, 2013
 *      Author: Xiaobing Zhou
 */

#include "lock_guard.h"

lock_guard::lock_guard(pthread_mutex_t *mutex) :
		_mutex(mutex) {

	lock();
}

lock_guard::~lock_guard() {

	unlock();
}

bool lock_guard::lock() {

	pthread_mutex_lock(_mutex);

	return true;
}

bool lock_guard::unlock() {

	pthread_mutex_unlock(_mutex);

	return true;
}
