/*
 * Watchdog.h
 *
 *  Created on: Jan 25, 2016
 *      Author: richard
 */

#ifndef INCLUDE_WATCHDOG_WATCHDOG_H_
#define INCLUDE_WATCHDOG_WATCHDOG_H_

#include <time.h>
#include <pthread.h>

namespace EventSystem {

class Watchdog {
public:
	/**
	 * initializes a watchdog object with a time span. If the watchdog reaches this span,
	 * a call to isTriggered() will return true; else it will return false.
	 * The time span can be rewinded with a call to set().
	 * @param trigTimeSec: time span in seconds
	 * @param trigTimeMilliSec: time span in milliseconds
	 */
	Watchdog(int trigTimeSec, int trigTimeMilliSec);
	virtual ~Watchdog();

	void set();
	bool isTriggered();

private:
	pthread_mutex_t memberMutex;
	struct timespec triggerTime;
	int sec;
	int millisec;
};

} /* namespace EventSystem */

#endif /* INCLUDE_WATCHDOG_WATCHDOG_H_ */
