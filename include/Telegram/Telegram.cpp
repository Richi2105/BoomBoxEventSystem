#include <stdint.h>
#include <string>
#include <OS_DEF.h>
#include "Telegram.h"

namespace EventSystem
{

EventSystemParticipant* Telegram::esp = nullptr;

std::string Telegram::ID_LOGGER = "LOGGER";
std::string Telegram::ID_MASTER = "MASTER";
std::string Telegram::ID_DISPLAY = "DISPLAY";
std::string Telegram::ID_DISPLAYCLIENT = "DISPLAYCLIENT";
std::string Telegram::ID_AUDIOPLAYER = "AUDIOPLAYER";
std::string Telegram::ID_INPUT = "INPUT";

const uint8 Telegram::ANONYMOUS = TELEGRAM_ANONYMOUS;
const uint8 Telegram::LOG = TELEGRAM_LOG;
const uint8 Telegram::REGISTER = TELEGRAM_REGISTER;
const uint8 Telegram::UNREGISTER = TELEGRAM_UNREGISTER;
const uint8 Telegram::REQUEST = TELEGRAM_REQUEST;
const uint8 Telegram::REQUESTANSWER = TELEGRAM_REQUESTANSWER;
const uint8 Telegram::PING = TELEGRAM_PING;
const uint8 Telegram::INPUT = TELEGRAM_INPUT;
const uint8 Telegram::DISPLAYDATA = TELEGRAM_DISPLAYDATA;
const uint8 Telegram::MEDIA = TELEGRAM_MEDIA;
const uint8 Telegram::QUIT = TELEGRAM_QUIT;

Telegram::Telegram(std::string identifier)
{
	memset(destinationID, 0, UNIQUEID_SIZE);
    memcpy(destinationID, identifier.c_str(), UNIQUEID_SIZE < identifier.size() ? UNIQUEID_SIZE : identifier.size());

	memset(sourceID, 0, UNIQUEID_SIZE);
	if (Telegram::esp != nullptr)
    {
		memcpy(sourceID, esp->getUniqueIdentifier().c_str(), UNIQUEID_SIZE < esp->getUniqueIdentifier().size() ? UNIQUEID_SIZE : esp->getUniqueIdentifier().size());
    }
    this->type = ANONYMOUS;
//    this->telegramSize = sizeof(Telegram);
    this->uniqueDestination = 0;
}

Telegram::Telegram()
{
	memset(destinationID, 0, UNIQUEID_SIZE);

	memset(sourceID, 0, UNIQUEID_SIZE);
	if (Telegram::esp != nullptr)
    {
		memcpy(sourceID, esp->getUniqueIdentifier().c_str(), UNIQUEID_SIZE < esp->getUniqueIdentifier().size() ? UNIQUEID_SIZE : esp->getUniqueIdentifier().size());
    }

    this->type = ANONYMOUS;
//    this->telegramSize = sizeof(Telegram);
    this->uniqueDestination = 0;
}

Telegram::~Telegram()
{
    //dtor
}

void Telegram::initTelegram(EventSystemParticipant* esp)
{
	Telegram::esp = esp;
}

void Telegram::setUniqueDestination(bool set)
{
	if (set)
		this->uniqueDestination = 1;
	else
		this->uniqueDestination = 0;
}

bool Telegram::isUniqueDestination()
{
	if (this->uniqueDestination == 0)
		return false;
	else
		return true;
}

char* Telegram::getDestinationID()
{
    return this->destinationID;
}

char* Telegram::getSourceID()
{
	return this->sourceID;
}

void Telegram::setIdentifier(std::string identifier)
{
	memset(destinationID, 0, UNIQUEID_SIZE);
    memcpy(destinationID, identifier.c_str(), UNIQUEID_SIZE < identifier.size() ? UNIQUEID_SIZE : identifier.size());
}

void Telegram::setSource(std::string source)
{
	memset(sourceID, 0, UNIQUEID_SIZE);
    memcpy(sourceID, source.c_str(), UNIQUEID_SIZE < source.size() ? UNIQUEID_SIZE : source.size());
}

//@DEPRECATED
int Telegram::getSize()
{
    //return this->telegramSize;
	return -1;
}

uint8 Telegram::getType()
{
	return this->type;
}

void Telegram::setType(uint8 type)
{
	this->type = type;
}

int Telegram::getSerializedSize()
{
	#ifdef DEBUG_OUT
	printf("Telegram::getSerializedSize()\n");
	#endif //DEBUG_OUT
	int16_t size = 0;
	size += sizeof(this->destinationID[0])*UNIQUEID_SIZE;
	size += sizeof(this->sourceID[0])*UNIQUEID_SIZE;
	size += sizeof(this->type);
	//size += sizeof(this->telegramSize);
	size += sizeof(this->uniqueDestination);

	return size;
}
int Telegram::serialize(void* const data)
{
	//void* data = malloc(this->getSerializedSize());
	#ifdef DEBUG_OUT
	printf("in Telegram::serialize(void* const data)\n");
	#endif //DEBUG_OUT
	MEMUNIT* data2 = (MEMUNIT*)data;
	packNData(data2, this->destinationID, UNIQUEID_SIZE);
	packNData(data2, this->sourceID, UNIQUEID_SIZE);
	packData(data2, this->type);
	//packData(data2, this->telegramSize);
	packData(data2, this->uniqueDestination);

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
	unpackNData(data2, this->destinationID, UNIQUEID_SIZE);
	unpackNData(data2, this->sourceID, UNIQUEID_SIZE);
	unpackData(data2, this->type);
	//unpackData(data2, this->telegramSize);
	unpackData(data2, this->uniqueDestination);

	#ifdef DEBUG_OUT
	printf("Offset data2 %p to data %p\n", data2, data);
	#endif //DEBUG_OUT

	return Telegram::getSerializedSize();
}

} /* namespace EventSystem */

