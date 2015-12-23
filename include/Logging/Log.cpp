/*
 * Log.cpp
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#include "Log.h"

namespace EventSystem {

Log::Log(EventSystemParticipant* source, std::string log, LoggerAdapter::level_t level)
{
    this->logtime = time(NULL);
    printf("New Log Telegram on %s\n", ctime(&(this->logtime)));
    memcpy(this->sourceID, source->getIdentifier().c_str(), source->getIdentifier().size());
    memcpy(this->uniqueSourceID, source->getUniqueIdentifier().c_str(), source->getUniqueIdentifier().size());
    memcpy(this->log, log.c_str(), log.size());
    this->level = level;
}

Log::Log()
{
	this->level = LoggerAdapter::INFO;
	this->logtime = 0;
}

Log::~Log()
{
	// TODO Auto-generated destructor stub
}

char* Log::getLog()
{
    return this->log;
}

void Log::setLog(std::string log, LoggerAdapter::level_t level)
{
	this->logtime = time(NULL);
	memcpy(this->log, log.c_str(), log.size());
	this->level = level;
}

time_t Log::getTime()
{
    return this->logtime;
}

char* Log::getSourceID()
{
    return this->sourceID;
}

char* Log::getUniqueSourceID()
{
    return this->uniqueSourceID;
}

LoggerAdapter::level_t Log::getLevel()
{
	return this->level;
}

int16_t Log::getSerializedSize()
{
	printf("Log::getSerializedSize()\n");
	int16_t size = 0;
	size += sizeof(this->log[0])*LOG_MESSAGE_SIZE;
	size += sizeof(this->sourceID[0])*ID_SIZE;
	size += sizeof(this->uniqueSourceID[0])*UNIQUEID_SIZE;
	size += sizeof(this->level);
	size += sizeof(this->logtime);

	return size;
}

int Log::serialize(void* const data)
{
	printf("Log::serialize(void* const data)\n");
	MEMUNIT* data2 = (MEMUNIT*)data;
	packNData(data2, this->log, LOG_MESSAGE_SIZE);
	packNData(data2, this->sourceID, ID_SIZE);
	packNData(data2, this->uniqueSourceID, UNIQUEID_SIZE);
	packData(data2, this->level);
	packData(data2, this->logtime);

	return this->getSerializedSize();
}

int Log::deserialize(void const * const data)
{
	printf("Log::deserialize(void const * const data)\n");
	const MEMUNIT* data2 = (MEMUNIT*)data;
	unpackNData(data2, this->log, LOG_MESSAGE_SIZE);
	unpackNData(data2, this->sourceID, ID_SIZE);
	unpackNData(data2, this->uniqueSourceID, UNIQUEID_SIZE);
	unpackData(data2, this->level);
	unpackData(data2, this->logtime);

	return this->getSerializedSize();
}

} /* namespace EventSystem */
