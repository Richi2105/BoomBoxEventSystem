/*
 * SocketAddressNetwork.cpp
 *
 *  Created on: Nov 20, 2015
 *      Author: richard
 */

#include <string.h>
#include <stdio.h>
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

