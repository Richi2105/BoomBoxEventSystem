/*
 * Log.cpp
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#include "Log.h"

namespace EventSystem {

Log::Log(EventSystemParticipant* source, std::string log, Log::level_t level)
{
    this->logtime = time(NULL);
	memset(this->sourceID, 0, ID_SIZE);
    memcpy(this->sourceID, source->getIdentifier().c_str(), ID_SIZE < source->getIdentifier().size() ? ID_SIZE : source->getIdentifier().size());

	memset(this->uniqueSourceID, 0, UNIQUEID_SIZE);
    memcpy(this->uniqueSourceID, source->getUniqueIdentifier().c_str(), UNIQUEID_SIZE < source->getUniqueIdentifier().size() ? UNIQUEID_SIZE : source->getUniqueIdentifier().size());

	memset(this->log, 0, LOG_MESSAGE_SIZE);
    memcpy(this->log, log.c_str(), LOG_MESSAGE_SIZE < log.size() ? LOG_MESSAGE_SIZE : log.size());
    this->level = level;
}

Log::Log()
{
	this->level = Log::INFO;
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

void Log::setLog(std::string log, Log::level_t level)
{
	this->logtime = time(NULL);
	memset(this->log, 0, LOG_MESSAGE_SIZE);
	memcpy(this->log, log.c_str(), LOG_MESSAGE_SIZE < log.size() ? LOG_MESSAGE_SIZE : log.size());
	this->level = level;
}

void Log::setSource(EventSystemParticipant* source)
{
	memset(this->sourceID, 0, ID_SIZE);
	memcpy(this->sourceID, source->getIdentifier().c_str(), ID_SIZE < source->getIdentifier().size() ? ID_SIZE : source->getIdentifier().size());

	memset(this->uniqueSourceID, 0, UNIQUEID_SIZE);
	memcpy(this->uniqueSourceID, source->getUniqueIdentifier().c_str(), UNIQUEID_SIZE < source->getUniqueIdentifier().size() ? UNIQUEID_SIZE : source->getUniqueIdentifier().size());
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

Log::level_t Log::getLevel()
{
	return this->level;
}

int Log::getSerializedSize()
{
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
	#ifdef DEBUG_OUT
	printf("Log::serialize(void* const data)\n");
	#endif //DEBUG_OUT
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
	#ifdef DEBUG_OUT
	printf("Log::deserialize(void const * const data)\n");
	#endif //DEBUG_OUT
	const MEMUNIT* data2 = (MEMUNIT*)data;
	unpackNData(data2, this->log, LOG_MESSAGE_SIZE);
	unpackNData(data2, this->sourceID, ID_SIZE);
	unpackNData(data2, this->uniqueSourceID, UNIQUEID_SIZE);
	unpackData(data2, this->level);
	unpackData(data2, this->logtime);

	return this->getSerializedSize();
}

} /* namespace EventSystem */
