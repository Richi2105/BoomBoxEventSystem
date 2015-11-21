/*
 * SocketAddressNetwork.h
 *
 *  Created on: Nov 20, 2015
 *      Author: richard
 */

#ifndef INCLUDE_SOCKETADDRESSNETWORK_H_
#define INCLUDE_SOCKETADDRESSNETWORK_H_

#include <netinet/in.h>
#include "SocketAddress.h"

class SocketAddressNetwork: public SocketAddress {
public:
	SocketAddressNetwork(sockaddr_in address, socklen_t len);
	SocketAddressNetwork();
	virtual ~SocketAddressNetwork();

	void setAddress(sockaddr_in address, socklen_t len);

	sockaddr* getAddress();
	socklen_t getLen();
	uint8_t getSize();

private:
	sockaddr_in address;
	socklen_t len;
	uint8_t addressSize;
};

#endif /* INCLUDE_SOCKETADDRESSNETWORK_H_ */
