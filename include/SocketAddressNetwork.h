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
#include "Serializeable.h"
#include "constants.h"

#include <string>

class SocketAddressNetwork: public SocketAddress, Serializeable
{
public:
	SocketAddressNetwork(sockaddr_in address, socklen_t len, std::string uid);
	SocketAddressNetwork();
	virtual ~SocketAddressNetwork();

	void setAddress(sockaddr_in address, socklen_t len);

	sockaddr* getAddress();
	socklen_t getLen();
	uint8_t getSize();

	void convertTo_Struct(void* address);

	bool isEqual(SocketAddressNetwork* address);

	bool operator==(SocketAddressNetwork* address);

	virtual char* getUniqueID();

	virtual int getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

private:
	sockaddr_in address;
	socklen_t len;
	uint8_t addressSize;
	char uniqueID[UNIQUEID_SIZE];
};

struct _socketAddress_network{
	sockaddr_in address;
	socklen_t len;
};

typedef struct _socketAddress_network socketAddress_network;

#endif /* INCLUDE_SOCKETADDRESSNETWORK_H_ */
