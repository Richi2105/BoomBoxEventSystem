/*
 * Log.h
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#ifndef INCLUDE_LOGGING_LOG_H_
#define INCLUDE_LOGGING_LOG_H_

#include <string>
#include <time.h>
#include "LoggerAdapter.h"
#include "../Serializeable.h"
#include "../Telegram/Telegram.h"
#include "../EventSystemParticipant.h"

#define LOG_MESSAGE_SIZE 250
#define UNIQUEID_SIZE 30

namespace EventSystem {

class Log : public Serializeable
{
public:
	Log(EventSystemParticipant* source, std::string log, LoggerAdapter::level_t level);
	Log();
	virtual ~Log();

	char* getLog();
	void setLog(std::string log, LoggerAdapter::level_t level);

	time_t getTime();
	char* getSourceID();
	char* getUniqueSourceID();
	LoggerAdapter::level_t getLevel();

	virtual int16_t getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

private:
    char log[LOG_MESSAGE_SIZE];
    char sourceID[ID_SIZE];
    char uniqueSourceID[UNIQUEID_SIZE];
    LoggerAdapter::level_t level;
    time_t logtime;
};

} /* namespace EventSystem */

#endif /* INCLUDE_LOGGING_LOG_H_ */
