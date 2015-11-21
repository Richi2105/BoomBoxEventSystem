/*
 * SocketAddressLocal.h
 *
 *  Created on: Nov 20, 2015
 *      Author: richard
 */

#ifndef INCLUDE_SOCKETADDRESSLOCAL_H_
#define INCLUDE_SOCKETADDRESSLOCAL_H_

#include <sys/un.h>

#include "SocketAddress.h"

class SocketAddressLocal: public SocketAddress {
public:
	SocketAddressLocal(sockaddr_un address, socklen_t len);
	SocketAddressLocal();
	virtual ~SocketAddressLocal();

	void setAddress(sockaddr_un address, socklen_t len);

	sockaddr* getAddress();
	socklen_t getLen();
	uint8_t getSize();

private:
	sockaddr_un address;
	socklen_t len;
	uint8_t addressSize;
};

#endif /* INCLUDE_SOCKETADDRESSLOCAL_H_ */
