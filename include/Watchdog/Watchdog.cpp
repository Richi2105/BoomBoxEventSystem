/*
 * Watchdog.cpp
 *
 *  Created on: Jan 25, 2016
 *      Author: richard
 */

#include "Watchdog.h"
#include <stdio.h>

namespace EventSystem {

Watchdog::Watchdog(int trigTimeSec, int trigTimeMilliSec) {
	this->sec = trigTimeSec;
	this->millisec = trigTimeMilliSec;
	pthread_mutex_init(&this->memberMutex, NULL);
}

Watchdog::~Watchdog() {
	pthread_mutex_destroy(&this->memberMutex);
}

void Watchdog::set()
{
	pthread_mutex_lock(&this->memberMutex);
	if (clock_gettime(CLOCK_REALTIME, &this->triggerTime) != 0)
	{
		fprintf(stderr, "Warning: not able to set time\n");
	}
	else
	{
		printf("Watchdog::set()\n");
		this->triggerTime.tv_nsec += this->millisec * 1000;
		this->triggerTime.tv_sec += this->sec;
	}
	pthread_mutex_unlock(&this->memberMutex);
}

bool Watchdog::isTriggered()
{
	bool retVal = false;
	struct timespec currTime;
	if (clock_gettime(CLOCK_REALTIME, &currTime) != 0)
	{
		fprintf(stderr, "Warning: not able to set time\n");
		retVal = false;
	}
	else
	{
		pthread_mutex_lock(&this->memberMutex);
		if (currTime.tv_sec > this->triggerTime.tv_sec)
		{
			retVal = true;
		}
		else if (currTime.tv_sec == this->triggerTime.tv_sec)
		{
			if (currTime.tv_nsec > this->triggerTime.tv_nsec)
			{
				retVal = true;
			}
		}
		pthread_mutex_unlock(&this->memberMutex);
	}
	return retVal;
}

} /* namespace EventSystem */
