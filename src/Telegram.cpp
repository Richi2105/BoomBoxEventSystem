#include <stdint.h>
#include <string>
#include <OS_DEF.h>
#include "../include/Telegram/Telegram.h"

namespace EventSystem
{

std::string Telegram::ID_LOGGER = "LOGGER";
std::string Telegram::ID_MASTER = "MASTER";
std::string Telegram::ID_DISPLAY = "DISPLAY";
std::string Telegram::ID_DISPLAYCLIENT = "DISPLAYCLIENT";
std::string Telegram::ID_AUDIOPLAYER = "AUDIOPLAYER";
std::string Telegram::ID_INPUT = "INPUT";

Telegram::Telegram(std::string identifier)
{
//            this->destinationID = identifier;
	memset(destinationID, 0, ID_SIZE);
    memcpy(destinationID, identifier.c_str(), ID_SIZE < identifier.size() ? ID_SIZE : identifier.size());
//            telegramSize = sizeof(char) * ID_SIZE + sizeof(int);
    this->type = ANONYMOUS;
    this->telegramSize = sizeof(Telegram);
}
Telegram::~Telegram()
{
    //dtor
}
char* Telegram::getDestinationID()
{
    return this->destinationID;
}

void Telegram::setIdentifier(std::string identifier)
{
	memset(destinationID, 0, ID_SIZE);
    memcpy(destinationID, identifier.c_str(), ID_SIZE < identifier.size() ? ID_SIZE : identifier.size());
}

//@DEPRECATED
int Telegram::getSize()
{
    return this->telegramSize;
}

Telegram::telegram_type Telegram::getType()
{
	return this->type;
}

void Telegram::setType(Telegram::telegram_type type)
{
	this->type = type;
}

int Telegram::getSerializedSize()
{
	#ifdef DEBUG_OUT
	printf("Telegram::getSerializedSize()\n");
	#endif //DEBUG_OUT
	int16_t size = 0;
	size += sizeof(this->destinationID[0])*ID_SIZE;
	size += sizeof(this->type);
	size += sizeof(this->telegramSize);

	return size;
}
int Telegram::serialize(void* const data)
{
	//void* data = malloc(this->getSerializedSize());
	#ifdef DEBUG_OUT
	printf("in Telegram::serialize(void* const data)\n");
	#endif //DEBUG_OUT
	MEMUNIT* data2 = (MEMUNIT*)data;
	packNData(data2, this->destinationID, ID_SIZE);
	packData(data2, this->type);
	packData(data2, this->telegramSize);

	#ifdef DEBUG_OUT
	printf("Offset data2 %p to data %p\n", data2, data);
	#endif //DEBUG_OUT

	return Telegram::getSerializedSize();
}
int Telegram::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	#ifdef DEBUG_OUT
	printf("in Telegram::deserialize(void const * const data)\n");
	#endif //DEBUG_OUT
	unpackNData(data2, this->destinationID, ID_SIZE);
	unpackData(data2, this->type);
	unpackData(data2, this->telegramSize);

	#ifdef DEBUG_OUT
	printf("Offset data2 %p to data %p\n", data2, data);
	#endif //DEBUG_OUT

	return Telegram::getSerializedSize();
}

} /* namespace EventSystem */

