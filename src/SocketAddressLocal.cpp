/*
 * SocketAddressLocal.cpp
 *
 *  Created on: Nov 20, 2015
 *      Author: richard
 */

#include <string.h>
#include <stdio.h>
#include <OS_DEF.h>
#include "../include/SocketAddressLocal.h"

SocketAddressLocal::SocketAddressLocal(sockaddr_un address, socklen_t len, std::string uid)
{
	printf("Creating SocketAddressLocal(sockaddr_un)\n"
			"Parameter: %s\n", address.sun_path);
    this->address = address;
    this->len = len;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_un);
    memset(this->uniqueID, 0, UNIQUEID_SIZE);
    memcpy(this->uniqueID, uid.c_str(), UNIQUEID_SIZE < uid.size() ? UNIQUEID_SIZE : uid.size());
}

SocketAddressLocal::SocketAddressLocal()
{
	printf("Creating SocketAddressLocal()\n");
//    memset(&(this->address), 0, sizeof(sockaddr_un));
    this->len = 0;
    this->addressSize = 0;
    memset(this->uniqueID, 0, UNIQUEID_SIZE);
}

SocketAddressLocal::~SocketAddressLocal()
{

}

void SocketAddressLocal::setAddress(sockaddr_un address, socklen_t len)
{
	printf("setting Address in SocketAddressLocal\n"
			"Parameter: %s\n", address.sun_path);
    this->address = address;
    this->len = len;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_un);
}

void SocketAddressLocal::setUID(std::string uid)
{
    memset(this->uniqueID, 0, UNIQUEID_SIZE);
    memcpy(this->uniqueID, uid.c_str(), UNIQUEID_SIZE < uid.size() ? UNIQUEID_SIZE : uid.size());
}

sockaddr* SocketAddressLocal::getAddress()
{
    return (sockaddr*)&(this->address);
}

socklen_t SocketAddressLocal::getLen()
{
    return this->len;
}

uint8_t SocketAddressLocal::getSize()
{
    return this->addressSize;
}

void SocketAddressLocal::convertTo_Struct(void* address)
{
	((socketAddress_local*)address)->address = this->address;
	((socketAddress_local*)address)->len = this->len;

}

bool SocketAddressLocal::isEqual(SocketAddress* address)
{
	if (address->isLocal())
	{
		SocketAddressLocal* add = (SocketAddressLocal*) address;
		int result = strcmp(this->address.sun_path, add->address.sun_path);
		result += strncmp(this->uniqueID, add->uniqueID, UNIQUEID_SIZE);
		if (result != 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
		return false;
}

bool SocketAddressLocal::operator==(SocketAddress* address)
{
	return this->isEqual(address);
}

char* SocketAddressLocal::getUniqueID()
{
	return this->uniqueID;
}

bool SocketAddressLocal::isLocal()
{
	return true;
}

int SocketAddressLocal::getSerializedSize()
{

	int16_t size = 0;
	size += sizeof(this->address);
	size += sizeof(this->addressSize);
	size += sizeof(this->len);
	size += sizeof(this->uniqueID[0]) * UNIQUEID_SIZE;

	return size;
}
int SocketAddressLocal::serialize(void* const data)
{
	//void* data = malloc(this->getSerializedSize());
	MEMUNIT* data2 = (MEMUNIT*)data;
	packData(data2, this->address.sun_family);
	packNData(data2, this->address.sun_path, 108);
	packData(data2, this->len);
	packData(data2, this->addressSize);
	packNData(data2, this->uniqueID, UNIQUEID_SIZE);

	return SocketAddressLocal::getSerializedSize();
}
int SocketAddressLocal::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;

	unpackData(data2, this->address.sun_family);
	unpackNData(data2, this->address.sun_path, 108);
	unpackData(data2, this->len);
	unpackData(data2, this->addressSize);
	unpackNData(data2, this->uniqueID, UNIQUEID_SIZE);

	return SocketAddressLocal::getSerializedSize();
}

