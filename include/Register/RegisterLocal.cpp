/*
 * RegisterLocal.cpp
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#include "RegisterLocal.h"

namespace EventSystem {

Register_Local::Register_Local()
{
	this->clientAddress = new SocketAddressLocal();
}

Register_Local::Register_Local(SocketAddressLocal* address, std::string clientName)
{
	this->clientAddress = address;
	memcpy(this->clientID, clientName.c_str(), clientName.size());
}

Register_Local::~Register_Local() {
	// TODO Auto-generated destructor stub
}

SocketAddressLocal* Register_Local::getClientAddress()
{
    return this->clientAddress;
}
char* Register_Local::getClientID()
{
    return this->clientID;
}

int16_t Register_Local::getSerializedSize()
{
	int16_t size = 0;
	size += sizeof(this->clientID[0])*ID_SIZE;
	size += this->clientAddress->getSerializedSize();

	return size;
}
int Register_Local::serialize(void* const data)
{
	printf("Register_Local::serialize(void* const data)\n");
	MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += this->clientAddress->serialize(data2);
	packNData(data2, this->clientID, ID_SIZE);

	return this->getSerializedSize();
}
int Register_Local::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	printf("in Register_Local::deserialize(void const * const data)\n");
	data2 += this->clientAddress->deserialize(data2);
	unpackNData(data2, this->clientID, ID_SIZE);

	return this->getSerializedSize();
}

} /* namespace EventSystem */
