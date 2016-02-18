/*
 * ClientWatchdog.h
 *
 *  Created on: Jan 26, 2016
 *      Author: richard
 */

#ifndef INCLUDE_WATCHDOG_CLIENTWATCHDOG_H_
#define INCLUDE_WATCHDOG_CLIENTWATCHDOG_H_

#include "../SocketAddress.h"

#include <time.h>

namespace EventSystem {

/**
 * a container which holds a reference to a client (aka SocketAddress)
 * and a time, at which the client did respond the last time.
 *
 * If a client did respond, call responded() to set it's time to the current one.
 *
 * call didRespondInTime to get the information if the client had a timeout.
 *
 * the timeout can be set globally via setResponseTime().
 */
class ClientWatchdog {
public:
	ClientWatchdog(SocketAddress* clientAddress);
	virtual ~ClientWatchdog();

	/**
	 * if the element is in a container (i.e. vector), set the positon
	 * of this element for easier access
	 * @param position: the position in the container,
	 * for a vector you could do the following:
	 * 		element->setPosition( vector.size() );
	 * 		vector.push_back(element);
	 */
	void setPosition(int position);

	int getPosition();

	/**
	 * set the respond time in milliseconds
	 * @param time: the relative time in milliseconds
	 */
	static void setResponseTime(double time);

	/**
	 * @return the client (aka SocketAddress*)
	 */
	SocketAddress* getClientAddress();

	/**
	 * to be called if a response was received. the lastResponse member will be
	 * updated to the current time.
	 */
	void responded();

	/**
	 * @return: whether the client did respond in the given response time.
	 * false: client did not respond
	 * true: client did respond
	 */
	bool didRespondInTime();

private:
	SocketAddress* clientAddress;
	time_t lastResponse;
	int position;
	static double responseTime;
};

} /* namespace EventSystem */

#endif /* INCLUDE_WATCHDOG_CLIENTWATCHDOG_H_ */
