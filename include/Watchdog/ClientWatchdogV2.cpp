/*
 * ClientWatchdogV2.cpp
 *
 *  Created on: Feb 17, 2016
 *      Author: richard
 */

#include "ClientWatchdogV2.h"

namespace EventSystem {

ClientWatchdogV2::ClientWatchdogV2(SocketAddress* address, int trigTimeSec, int trigTimeMilliSec) : Watchdog(trigTimeSec, trigTimeMilliSec)
{
	this->client = address;
}

ClientWatchdogV2::~ClientWatchdogV2()
{
	delete this->client;
}

SocketAddress* ClientWatchdogV2::getClientAddress()
{
	return this->client;
}

} /* namespace EventSystem */
