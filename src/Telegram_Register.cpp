#include "../include/Telegram/Telegram_Register.h"

Telegram_Register::Telegram_Register(SocketAddressLocal clientAddress, std::string id) : Telegram("MASTER")
{
    this->clientAddress = clientAddress;
    memcpy(this->clientID, id.c_str(), id.size());
 //   this->telegramSize += this->clientAddress.getSize() + sizeof(char)*ID_SIZE;
    this->telegramSize += sizeof(Telegram_Register);
//            printf("Telegram initialized; source ID: %s\n", this->getClientID());
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

void initTelegram_Register_Local(telegram_register_local* telegram, socketAddress_local address, std::string clientID)
{
	initTelegram(&telegram->header, "MASTER");
	telegram->header.telegramSize = sizeof(telegram_register_local);
	telegram->address = address;
	memcpy(telegram->clientID, clientID.c_str(), ID_SIZE < clientID.size() ? ID_SIZE : clientID.size());
}
