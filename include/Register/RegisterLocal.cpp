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
	memset(this->clientID, 0, ID_SIZE);
    memcpy(this->clientID, clientName.c_str(), ID_SIZE < clientName.size() ? ID_SIZE : clientName.size());
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

int Register_Local::getSerializedSize()
{
	int16_t size = 0;
	size += sizeof(this->clientID[0])*ID_SIZE;
	size += this->clientAddress->getSerializedSize();

	return size;
}
int Register_Local::serialize(void* const data)
{
	#ifdef DEBUG_OUT
	printf("Register_Local::serialize(void* const data)\n");
	#endif //DEBUG_OUT
	MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += this->clientAddress->serialize(data2);
	packNData(data2, this->clientID, ID_SIZE);

	return this->getSerializedSize();
}
int Register_Local::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	#ifdef DEBUG_OUT
	printf("in Register_Local::deserialize(void const * const data)\n");
	#endif //DEBUG_OUT
	data2 += this->clientAddress->deserialize(data2);
	unpackNData(data2, this->clientID, ID_SIZE);

	return this->getSerializedSize();
}

} /* namespace EventSystem */
