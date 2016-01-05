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
#include <OS_DEF.h>
#include "../Serializeable.h"
#include "../Telegram/Telegram.h"
#include "../EventSystemParticipant.h"


namespace EventSystem {

class Log : public Serializeable
{
public:
	enum level_t {INFO, WARNING, SEVERE};

	Log(EventSystemParticipant* source, std::string log, Log::level_t level);
	Log();
	virtual ~Log();

	char* getLog();
	void setLog(std::string log, Log::level_t level);

	void setSource(EventSystemParticipant* source);

	time_t getTime();
	char* getSourceID();
	char* getUniqueSourceID();
	Log::level_t getLevel();

	virtual int getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

private:
    char log[LOG_MESSAGE_SIZE];
    char sourceID[ID_SIZE];
    char uniqueSourceID[UNIQUEID_SIZE];
    Log::level_t level;
    int64 logtime;
};

} /* namespace EventSystem */

#endif /* INCLUDE_LOGGING_LOG_H_ */
