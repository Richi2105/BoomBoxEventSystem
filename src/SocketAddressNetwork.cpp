/*
 * SocketAddressNetwork.cpp
 *
 *  Created on: Nov 20, 2015
 *      Author: richard
 */

#include <string.h>
#include <stdio.h>
#include <OS_DEF.h>
#include "../include/SocketAddressNetwork.h"

SocketAddressNetwork::SocketAddressNetwork(sockaddr_in address, socklen_t len, std::string uid)
{
	#ifdef DEBUG_OUT
	printf("Creating SocketAddressLocal(sockaddr_in)\n"
			"Parameter: %x", address.sin_addr.s_addr);
	#endif //DEBUG_OUT
    this->address = address;
    this->len = len;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_in);
    memset(this->uniqueID, 0, UNIQUEID_SIZE);
    memcpy(this->uniqueID, uid.c_str(), UNIQUEID_SIZE < uid.size() ? UNIQUEID_SIZE : uid.size());
}

SocketAddressNetwork::SocketAddressNetwork()
{
	#ifdef DEBUG_OUT
	printf("Creating SocketAddressNetwork()\n");
	#endif //DEBUG_OUT
    memset(&(this->address), 0, sizeof(sockaddr_in));
    this->len = 0;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_in);
}

SocketAddressNetwork::~SocketAddressNetwork()
{

}

void SocketAddressNetwork::setAddress(sockaddr_in address, socklen_t len)
{
	#ifdef DEBUG_OUT
	printf("setting Address in SocketAddressLocal\n"
			"Parameter: %x\n", address.sin_addr.s_addr);
	#endif //DEBUG_OUT
    this->address = address;
    this->len = len;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_in);
}

void SocketAddressNetwork::setUID(std::string uid)
{
    memset(this->uniqueID, 0, UNIQUEID_SIZE);
    memcpy(this->uniqueID, uid.c_str(), UNIQUEID_SIZE < uid.size() ? UNIQUEID_SIZE : uid.size());
}

sockaddr* SocketAddressNetwork::getAddress()
{
    return (sockaddr*)&(this->address);
}

socklen_t SocketAddressNetwork::getLen()
{
    return this->len;
}

uint8_t SocketAddressNetwork::getSize()
{
    return this->addressSize;
}

void SocketAddressNetwork::convertTo_Struct(void* address)
{
	((socketAddress_network*)address)->address = this->address;
	((socketAddress_network*)address)->len = this->len;
}

bool SocketAddressNetwork::isEqual(SocketAddress* address)
{
	if (address->isLocal())
		return false;
	else
	{
		SocketAddressNetwork* add = (SocketAddressNetwork*) address;
		if (this->address.sin_addr.s_addr != add->address.sin_addr.s_addr
				&& strncmp(this->uniqueID, add->uniqueID, UNIQUEID_SIZE) != 0)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

bool SocketAddressNetwork::operator==(SocketAddress* address)
{
	return this->isEqual(address);
}

char* SocketAddressNetwork::getUniqueID()
{
	return this->uniqueID;
}

bool SocketAddressNetwork::isLocal()
{
	return false;
}

int SocketAddressNetwork::getSerializedSize()
{
	int16_t size = 0;
	size += sizeof(this->address.sin_addr.s_addr);
	size += sizeof(this->address.sin_family);
	size += sizeof(this->address.sin_port);

	size += sizeof(this->addressSize);
	size += sizeof(this->len);
	size += sizeof(this->uniqueID[0]) * UNIQUEID_SIZE;

	return size;
}
int SocketAddressNetwork::serialize(void* const data)
{
	//void* data = malloc(this->getSerializedSize());
	MEMUNIT* data2 = (MEMUNIT*)data;
	packData(data2, this->address.sin_addr.s_addr);
	packData(data2, this->address.sin_family);
	packData(data2, this->address.sin_port);

	packData(data2, this->len);
	packData(data2, this->addressSize);
	packNData(data2, this->uniqueID, UNIQUEID_SIZE);
	return this->getSerializedSize();
}
int SocketAddressNetwork::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	unpackData(data2, this->address.sin_addr.s_addr);
	unpackData(data2, this->address.sin_family);
	unpackData(data2, this->address.sin_port);

	unpackData(data2, this->len);
	unpackData(data2, this->addressSize);
	unpackNData(data2, this->uniqueID, UNIQUEID_SIZE);
	return this->getSerializedSize();
}

