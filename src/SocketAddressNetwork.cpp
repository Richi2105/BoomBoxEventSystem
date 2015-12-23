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

SocketAddressNetwork::SocketAddressNetwork(sockaddr_in address, socklen_t len)
{
	printf("Creating SocketAddressLocal(sockaddr_in)\n"
			"Parameter: %x", address.sin_addr.s_addr);
    this->address = address;
    this->len = len;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_in);
}

SocketAddressNetwork::SocketAddressNetwork()
{
	printf("Creating SocketAddressNetwork()\n");
    memset(&(this->address), 0, sizeof(sockaddr_in));
    this->len = 0;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_in);
}

SocketAddressNetwork::~SocketAddressNetwork()
{

}

void SocketAddressNetwork::setAddress(sockaddr_in address, socklen_t len)
{
	printf("setting Address in SocketAddressLocal\n"
			"Parameter: %x\n", address.sin_addr.s_addr);
    this->address = address;
    this->len = len;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_in);
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

bool SocketAddressNetwork::isEqual(SocketAddressNetwork* address)
{
	if (this->address.sin_addr.s_addr != address->address.sin_addr.s_addr)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool SocketAddressNetwork::operator==(SocketAddressNetwork* address)
{
	return this->isEqual(address);
}

int16_t SocketAddressNetwork::getSerializedSize()
{
	int16_t size = 0;
	size += sizeof(this->address.sin_addr.s_addr);
	size += sizeof(this->address.sin_family);
	size += sizeof(this->address.sin_port);

	size += sizeof(this->addressSize);
	size += sizeof(this->len);

	return size;
}
int SocketAddressNetwork::serialize(void* const data)
{
	//void* data = malloc(this->getSerializedSize());
	MEMUNIT* data2 = (MEMUNIT*)data;
	printf("in fuction serialize()\nPointer to data: %p\n", data);
	packData(data2, this->address.sin_addr.s_addr);
	packData(data2, this->address.sin_family);
	packData(data2, this->address.sin_port);

	packData(data2, this->len);
	packData(data2, this->addressSize);
	return this->getSerializedSize();
}
int SocketAddressNetwork::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	printf("in fuction deserialize()\nPointer to data: %p\n", data2);
	unpackData(data2, this->address.sin_addr.s_addr);
	unpackData(data2, this->address.sin_family);
	unpackData(data2, this->address.sin_port);

	unpackData(data2, this->len);
	unpackData(data2, this->addressSize);
	return this->getSerializedSize();
}

