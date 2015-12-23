#ifndef TELEGRAM_LOG_H
#define TELEGRAM_LOG_H

#include <string>
#include <time.h>
#include <stdio.h>

#include "../constants.h"

#include "Telegram.h"
#include "../EventSystemParticipant.h"
#include "../Logging/LoggerAdapter.h"

class Telegram_Log : public Telegram::Telegram
{
    public:
        Telegram_Log(EventSystemParticipant* source, std::string log, LoggerAdapter::level_t level);
        Telegram_Log();
        virtual ~Telegram_Log();
        char* getLog();
        void setLog(std::string log, LoggerAdapter::level_t level);
        time_t getTime();
        char* getSourceID();
        char* getUniqueSourceID();
        LoggerAdapter::level_t getLevel();

        virtual int16_t getSerializedSize();
    	virtual int serialize(void* const data);
    	virtual int deserialize(void const * const data);

    protected:
    private:
/*        std::string log;
        std::string sourceID;
        std::string uniqueSourceID;*/
        char log[LOG_MESSAGE_SIZE];
        char sourceID[ID_SIZE];
        char uniqueSourceID[UNIQUEID_SIZE];
        LoggerAdapter::level_t level;
        time_t logtime;
};

struct _telegram_log{
	telegram_head header;
	int8_t log[LOG_MESSAGE_SIZE];
	time_t logTime;
	int8_t sourceID[ID_SIZE];
	int8_t uniqueSourceID[UNIQUEID_SIZE];
};

typedef struct _telegram_log telegram_log;

void initTelegram_log(telegram_log* telegram, EventSystemParticipant* source, std::string* log);
void setLog_Telegram_log(telegram_log* telegram, std::string* log);

#endif // TELEGRAM_LOG_H
