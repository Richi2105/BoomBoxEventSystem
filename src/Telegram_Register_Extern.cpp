#include "../include/Telegram/Telegram_Register_Extern.h"

Telegram_Register_Extern::Telegram_Register_Extern(SocketAddressNetwork clientAddress, std::string id) : Telegram("MASTER")
{
    this->clientAddress = clientAddress;
    memcpy(this->clientID, id.c_str(), id.size());
 //   this->telegramSize += this->clientAddress.getSize() + sizeof(char)*ID_SIZE;
    this->telegramSize += sizeof(Telegram_Register_Extern);
//            printf("Telegram initialized; source ID: %s\n", this->getClientID());
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