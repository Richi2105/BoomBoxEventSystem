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
#include "Serializeable.h"

class SocketAddressLocal: public SocketAddress, Serializeable
{
public:
	SocketAddressLocal(sockaddr_un address, socklen_t len);
	SocketAddressLocal();
	virtual ~SocketAddressLocal();

	void setAddress(sockaddr_un address, socklen_t len);

	virtual sockaddr* getAddress();
	virtual socklen_t getLen();
	virtual uint8_t getSize();

	virtual void convertTo_Struct(void* address);

	virtual int16_t getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

private:
	sockaddr_un address;
	socklen_t len;
	uint8_t addressSize;
};

struct _socketAddressLocal{
	sockaddr_un address;
	socklen_t len;
};

typedef struct _socketAddressLocal socketAddress_local;

#endif /* INCLUDE_SOCKETADDRESSLOCAL_H_ */
