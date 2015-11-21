/*
 * SocketAddressLocal.cpp
 *
 *  Created on: Nov 20, 2015
 *      Author: richard
 */

#include <string.h>
#include "../include/SocketAddressLocal.h"

SocketAddressLocal::SocketAddressLocal(sockaddr_un address, socklen_t len)
{
    this->address = address;
    this->len = len;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_un);
}

SocketAddressLocal::SocketAddressLocal()
{
    memset(&(this->address), 0, sizeof(sockaddr_un));
    this->len = 0;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(sockaddr_un);
}

SocketAddressLocal::~SocketAddressLocal()
{

}

void SocketAddressLocal::setAddress(sockaddr_un address, socklen_t len)
{
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
