/*
 * RegisterNetwork.cpp
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#include "RegisterNetwork.h"

namespace EventSystem {

Register_Network::Register_Network()
{
	this->clientAddress = new SocketAddressNetwork();
}

Register_Network::Register_Network(SocketAddressNetwork* address, std::string clientName)
{
	this->clientAddress = address;
	memcpy(this->clientID, clientName.c_str(), clientName.size());
}

Register_Network::~Register_Network() {
	// TODO Auto-generated destructor stub
}

SocketAddressNetwork* Register_Network::getClientAddress()
{
    return this->clientAddress;
}
char* Register_Network::getClientID()
{
    return this->clientID;
}

int16_t Register_Network::getSerializedSize()
{
	int16_t size = 0;
	size += sizeof(this->clientID[0])*ID_SIZE;
	size += this->clientAddress->getSerializedSize();

	return size;
}
int Register_Network::serialize(void* const data)
{
	printf("Register_Network::serialize(void* const data)\n");
	MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += this->clientAddress->serialize(data2);
	packNData(data2, this->clientID, ID_SIZE);

	return this->getSerializedSize();
}
int Register_Network::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	printf("in Register_Network::deserialize(void const * const data)\n");
	data2 += this->clientAddress->deserialize(data2);
	unpackNData(data2, this->clientID, ID_SIZE);

	return this->getSerializedSize();
}

} /* namespace EventSystem */
