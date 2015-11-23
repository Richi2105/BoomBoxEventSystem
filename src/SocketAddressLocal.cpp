/*
 * SocketAddressLocal.cpp
 *
 *  Created on: Nov 20, 2015
 *      Author: richard
 */

#include <string.h>
#include <stdio.h>
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
    memset(&(this->address), 0, sizeof(sockaddr_un));
    this->len = 0;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_un);
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
