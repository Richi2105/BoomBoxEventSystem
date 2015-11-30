#include "../include/Telegram/Telegram_Register.h"
#include <OS_DEF.h>

Telegram_Register::Telegram_Register(SocketAddressLocal clientAddress, std::string id) : Telegram("MASTER")
{
    this->clientAddress = clientAddress;
    memcpy(this->clientID, id.c_str(), id.size());
 //   this->telegramSize += this->clientAddress.getSize() + sizeof(char)*ID_SIZE;
    this->telegramSize += sizeof(Telegram_Register);
//            printf("Telegram initialized; source ID: %s\n", this->getClientID());
}
Telegram_Register::Telegram_Register() : Telegram("MASTER"), clientAddress()
{

}
Telegram_Register::~Telegram_Register() {}
SocketAddressLocal* Telegram_Register::getClientAddress()
{
    return &(this->clientAddress);
}
char* Telegram_Register::getClientID()
{
    return this->clientID;
}

int16_t Telegram_Register::getSerializedSize()
{
	printf("Telegram_Register::getSerializedSize()\n");
	int16_t size = 0;
	size += Telegram::getSerializedSize();
	size += sizeof(this->clientID[0])*ID_SIZE;
	size += this->clientAddress.getSerializedSize();

	return size;
}
int Telegram_Register::serialize(void* const data)
{
	//void* data = malloc(this->getSerializedSize());
	printf("Telegram_Register::serialize(void* const data)\n");
	MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::serialize(data2);
	data2 += this->clientAddress.serialize(data2);
	packNData(data2, this->clientID, ID_SIZE);

	return Telegram_Register::getSerializedSize();
}
int Telegram_Register::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	printf("in Telegram_Register::deserialize(void const * const data)\n");
	data2 += Telegram::deserialize(data2);
	data2 += this->clientAddress.deserialize(data2);
	unpackNData(data2, this->clientID, ID_SIZE);

	return Telegram_Register::getSerializedSize();
}

void initTelegram_Register_Local(telegram_register_local* telegram, socketAddress_local address, std::string* clientID)
{
	initTelegram(&telegram->header, new std::string("MASTER"));
	telegram->header.telegramSize = sizeof(telegram_register_local);
	telegram->address = address;
	memcpy(telegram->clientID, clientID->c_str(), ID_SIZE < clientID->size() ? ID_SIZE : clientID->size());
}
