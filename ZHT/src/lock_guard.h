/*
 * lock_guard.h
 *
 *  Created on: Apr 24, 2013
 *      Author: Xiaobing Zhou
 */

#ifndef LOCK_GUARD_H_
#define LOCK_GUARD_H_

#include <pthread.h>
#include <stdlib.h>

/*
 *
 */
class lock_guard {
public:
	lock_guard(pthread_mutex_t *mutex);
	virtual ~lock_guard();

private:
	bool lock();
	bool unlock();

private:
	pthread_mutex_t *_mutex;
};

#endif /* LOCK_GUARD_H_ */
