/*
 * ClientWatchdogV2.h
 *
 *  Created on: Feb 17, 2016
 *      Author: richard
 */

#ifndef INCLUDE_WATCHDOG_CLIENTWATCHDOGV2_H_
#define INCLUDE_WATCHDOG_CLIENTWATCHDOGV2_H_

#include "Watchdog.h"
#include "../SocketAddress.h"

namespace EventSystem {

class ClientWatchdogV2: public Watchdog {
public:
	ClientWatchdogV2(SocketAddress* address, int trigTimeSec, int trigTimeMilliSec);
	virtual ~ClientWatchdogV2();

	/**
	 * @return the client (aka SocketAddress*)
	 */
	SocketAddress* getClientAddress();

private:
	SocketAddress* client;
};

} /* namespace EventSystem */

#endif /* INCLUDE_WATCHDOG_CLIENTWATCHDOGV2_H_ */
