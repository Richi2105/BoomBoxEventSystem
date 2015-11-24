#include <stdint.h>
#include <string>
#include "../include/Telegram/Telegram.h"

Telegram::Telegram(std::string identifier)
{
//            this->destinationID = identifier;
    memcpy(destinationID, identifier.c_str(), identifier.size());
//            telegramSize = sizeof(char) * ID_SIZE + sizeof(int);
    telegramSize = sizeof(Telegram);
}
Telegram::~Telegram()
{
    //dtor
}
char* Telegram::getDestinationID()
{
    return this->destinationID;
}
int Telegram::getSize()
{
    return this->telegramSize;
}

void initTelegram(telegram* telegram, std::string id)
{
	memcpy(telegram->destinationID, id.c_str(), ID_SIZE < id.size() ? ID_SIZE : id.size());
	telegram->telegramSize = sizeof(struct _telegram);
}
