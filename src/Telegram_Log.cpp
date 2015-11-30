#include "../include/Telegram/Telegram_Log.h"
#include <OS_DEF.h>

Telegram_Log::Telegram_Log(EventSystemParticipant* source, std::string log) : Telegram("LOGGER")
{
    this->logtime = time(NULL);
    printf("New Log Telegram on %s\n", ctime(&(this->logtime)));
    //this->telegramSize += sizeof(time_t);
    memcpy(this->sourceID, source->getIdentifier().c_str(), source->getIdentifier().size());
//            this->sourceID = source->getIdentifier();
    //this->telegramSize += sizeof(char)*ID_SIZE;
    memcpy(this->uniqueSourceID, source->getUniqueIdentifier().c_str(), source->getUniqueIdentifier().size());
//            this->uniqueSourceID = source->getUniqueIdentifier();
    //this->telegramSize += sizeof(char)*UNIQUEID_SIZE;
    memcpy(this->log, log.c_str(), log.size());
//            this->log = log;
    //this->telegramSize += sizeof(char)*LOG_MESSAGE_SIZE;
    this->telegramSize += sizeof(Telegram_Log);

}
Telegram_Log::Telegram_Log() : Telegram("LOGGER")
{
	this->logtime = time(NULL);
}
Telegram_Log::~Telegram_Log() {}
char* Telegram_Log::getLog()
{
    return this->log;
}
time_t Telegram_Log::getTime()
{
    return this->logtime;
}
char* Telegram_Log::getSourceID()
{
    return this->sourceID;
}
char* Telegram_Log::getUniqueSourceID()
{
    return this->uniqueSourceID;
}

int16_t Telegram_Log::getSerializedSize()
{
	printf("Telegram_Log::getSerializedSize()\n");
	int16_t size = 0;
	size += Telegram::getSerializedSize();
	size += sizeof(this->log[0])*LOG_MESSAGE_SIZE;
	size += sizeof(this->sourceID[0])*ID_SIZE;
	size += sizeof(this->uniqueSourceID[0])*UNIQUEID_SIZE;
	size += sizeof(this->logtime);

	return size;
}
int Telegram_Log::serialize(void* const data)
{
	printf("Telegram_Log::serialize(void* const data)\n");
	//void* data = malloc(this->getSerializedSize());
	MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::serialize(data2);
	packNData(data2, this->log, LOG_MESSAGE_SIZE);
	packNData(data2, this->sourceID, ID_SIZE);
	packNData(data2, this->uniqueSourceID, UNIQUEID_SIZE);
	packData(data2, this->logtime);

	return this->getSerializedSize();
}
int Telegram_Log::deserialize(void const * const data)
{
	printf("Telegram_Log::deserialize(void const * const data)\n");
	const MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::deserialize(data2);
	unpackNData(data2, this->log, LOG_MESSAGE_SIZE);
	unpackNData(data2, this->sourceID, ID_SIZE);
	unpackNData(data2, this->uniqueSourceID, UNIQUEID_SIZE);
	unpackData(data2, this->logtime);

	return this->getSerializedSize();
}

void initTelegram_log(telegram_log* telegram, EventSystemParticipant* source, std::string* log)
{
	initTelegram(&telegram->header, new std::string("LOGGER"));
	telegram->header.telegramSize = sizeof(telegram_log);
	memcpy(telegram->sourceID, source->getIdentifier().c_str(), source->getIdentifier().size());
	memcpy(telegram->uniqueSourceID, source->getUniqueIdentifier().c_str(), source->getUniqueIdentifier().size());
	memcpy(telegram->log, log->c_str(), log->size());
	telegram->logTime = time(NULL);
}

void setLog_Telegram_log(telegram_log* telegram, std::string* log)
{
	memcpy(telegram->log, log->c_str(), log->size());
	telegram->logTime = time(NULL);
}
