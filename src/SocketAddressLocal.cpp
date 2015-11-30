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

SocketAddressLocal::SocketAddressLocal(sockaddr_un address, socklen_t len)
{
	printf("Creating SocketAddressLocal(sockaddr_un)\n"
			"Parameter: %s\n", address.sun_path);
    this->address = address;
    this->len = len;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_un);
}

SocketAddressLocal::SocketAddressLocal()
{
	printf("Creating SocketAddressLocal()\n");
//    memset(&(this->address), 0, sizeof(sockaddr_un));
    this->len = 0;
    this->addressSize = 0;
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

int16_t SocketAddressLocal::getSerializedSize()
{
	printf("in SocketAddressLocal::getSerializedSize()\n");
	int16_t size = 0;
	size += sizeof(this->address);
	size += sizeof(this->addressSize);
	size += sizeof(this->len);

	printf("size is %d\n", size);

	return size;
}
int SocketAddressLocal::serialize(void* const data)
{
	//void* data = malloc(this->getSerializedSize());
	MEMUNIT* data2 = (MEMUNIT*)data;
	printf("in SocketAddressLocal::serialize(void* const data)\n");
		//packData(data2, this->address);
	packData(data2, this->address.sun_family);
	packNData(data2, this->address.sun_path, 108);
	packData(data2, this->len);
	packData(data2, this->addressSize);

	printf("Offset data2 %p to data %p\n", data2, data);

	return SocketAddressLocal::getSerializedSize();
}
int SocketAddressLocal::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	printf("in SocketAddressLocal::deserialize(void const * const data)\n");
	unpackData(data2, this->address.sun_family);
	unpackNData(data2, this->address.sun_path, 108);
	unpackData(data2, this->len);
	unpackData(data2, this->addressSize);

	printf("Offset data2 %p to data %p\n", data2, data);

	return SocketAddressLocal::getSerializedSize();
}

