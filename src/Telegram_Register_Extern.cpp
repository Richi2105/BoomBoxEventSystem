#include "../include/Telegram/Telegram_Register_Extern.h"
#include <OS_DEF.h>

Telegram_Register_Extern::Telegram_Register_Extern(SocketAddressNetwork clientAddress, std::string id) : Telegram("MASTER")
{
    this->clientAddress = clientAddress;
    memcpy(this->clientID, id.c_str(), id.size());
 //   this->telegramSize += this->clientAddress.getSize() + sizeof(char)*ID_SIZE;
    this->telegramSize += sizeof(Telegram_Register_Extern);
//            printf("Telegram initialized; source ID: %s\n", this->getClientID());
}
Telegram_Register_Extern::Telegram_Register_Extern() : Telegram("MASTER")
{

}
Telegram_Register_Extern::~Telegram_Register_Extern() {}
SocketAddressNetwork* Telegram_Register_Extern::getClientAddress()
{
    return &(this->clientAddress);
}
char* Telegram_Register_Extern::getClientID()
{
    return this->clientID;
}

int16_t Telegram_Register_Extern::getSerializedSize()
{
	printf("Telegram_Register_Extern::getSerializedSize()\n");
	int16_t size = 0;
	size += Telegram::getSerializedSize();
	size += sizeof(this->clientID[0])*ID_SIZE;
	size += this->clientAddress.getSerializedSize();

	return size;
}
int Telegram_Register_Extern::serialize(void* const data)
{
	//void* data = malloc(this->getSerializedSize());
	printf("Telegram_Register_Extern::serialize(void* const data)\n");
	MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::serialize(data2);
	data2 += this->clientAddress.serialize(data2);
	packNData(data2, this->clientID, ID_SIZE);

	return this->getSerializedSize();
}
int Telegram_Register_Extern::deserialize(void const * const data)
{
	printf("Telegram_Register_Extern::deserialize(void const * const data)\n");
	const MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::deserialize(data2);
	data2 += this->clientAddress.deserialize(data2);
	unpackNData(data2, this->clientID, ID_SIZE);

	return this->getSerializedSize();
}

void initTelegram_Register_Network(telegram_register_network* telegram, socketAddress_network address, std::string* clientID)
{
	initTelegram(&telegram->header, new std::string("MASTER"));
	telegram->header.telegramSize = sizeof(telegram_register_network);
	telegram->address = address;
	memcpy(telegram->clientID, clientID->c_str(), ID_SIZE < clientID->size() ? ID_SIZE : clientID->size());
}
