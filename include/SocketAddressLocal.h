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
#include "constants.h"

#include <string>

class SocketAddressLocal : public SocketAddress, Serializeable
{
public:
	SocketAddressLocal(sockaddr_un address, socklen_t len, std::string uid);
	SocketAddressLocal();
	virtual ~SocketAddressLocal();

	void setAddress(sockaddr_un address, socklen_t len);
	void setUID(std::string uid);

	virtual sockaddr* getAddress();
	virtual socklen_t getLen();
	virtual uint8_t getSize();

	virtual void convertTo_Struct(void* address);

	virtual bool isEqual(SocketAddress* address);

	virtual bool operator==(SocketAddress* address);

	virtual char* getUniqueID();

	virtual bool isLocal();

	virtual int getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

private:
	sockaddr_un address;
	socklen_t len;
	uint8_t addressSize;
	char uniqueID[UNIQUEID_SIZE];
};

struct _socketAddressLocal{
	sockaddr_un address;
	socklen_t len;
};

typedef struct _socketAddressLocal socketAddress_local;

#endif /* INCLUDE_SOCKETADDRESSLOCAL_H_ */
