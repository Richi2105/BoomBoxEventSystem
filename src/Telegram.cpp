#include <stdint.h>
#include <string>
#include <OS_DEF.h>
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

int16_t Telegram::getSerializedSize()
{
	printf("Telegram::getSerializedSize()\n");
	int16_t size = 0;
	size += sizeof(this->destinationID[0])*ID_SIZE;
	size += sizeof(this->telegramSize);

	return size;
}
int Telegram::serialize(void* const data)
{
	//void* data = malloc(this->getSerializedSize());
	printf("in Telegram::serialize(void* const data)\n");
	MEMUNIT* data2 = (MEMUNIT*)data;
	packNData(data2, this->destinationID, ID_SIZE);
	packData(data2, this->telegramSize);

	printf("Offset data2 %p to data %p\n", data2, data);

	return Telegram::getSerializedSize();
}
int Telegram::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	printf("in Telegram::deserialize(void const * const data)\n");
	unpackNData(data2, this->destinationID, ID_SIZE);
	unpackData(data2, this->telegramSize);

	printf("Offset data2 %p to data %p\n", data2, data);

	return Telegram::getSerializedSize();
}

void initTelegram(telegram_head* telegram, std::string* id)
{
	memcpy(telegram->destinationID, id->c_str(), ID_SIZE < id->size() ? ID_SIZE : id->size());
	telegram->telegramSize = sizeof(struct _telegram);
}
