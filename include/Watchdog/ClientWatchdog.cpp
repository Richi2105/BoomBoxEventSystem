/*
 * ClientWatchdog.cpp
 *
 *  Created on: Jan 26, 2016
 *      Author: richard
 */

#include "ClientWatchdog.h"
#include <stdio.h>

namespace EventSystem {

double ClientWatchdog::responseTime = 0;

ClientWatchdog::ClientWatchdog(SocketAddress* clientAddress) {
	this->clientAddress = clientAddress;
	this->position = 0;
	this->lastResponse = time(NULL);
}

ClientWatchdog::~ClientWatchdog() {
	delete this->clientAddress;
}

void ClientWatchdog::setResponseTime(double time)
{
	ClientWatchdog::responseTime = time;
}

SocketAddress* ClientWatchdog::getClientAddress()
{
	return this->clientAddress;
}

void ClientWatchdog::responded()
{
	this->lastResponse = time(NULL);
}

bool ClientWatchdog::didRespondInTime()
{
	printf("in ClientWatchdog::didRespondInTime()\n"
			"response time: %f, difftime: %f (%ld - %ld)\n", ClientWatchdog::responseTime, difftime(time(NULL), this->lastResponse), time(NULL), this->lastResponse);
	if ( (ClientWatchdog::responseTime) < (difftime(time(NULL), this->lastResponse)) )
	{
		printf("->False\n");
		return false;
	}
	else
	{
		printf("->True\n");
		return true;
	}
}

} /* namespace EventSystem */
