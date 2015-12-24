#ifndef TELEGRAM_LOG_H
#define TELEGRAM_LOG_H

#include <string>
#include <time.h>
#include <stdio.h>

#include "../constants.h"

#include "Telegram.h"
#include "../EventSystemParticipant.h"
#include "../Logging/LoggerAdapter.h"

class Telegram_Log : public EventSystem::Telegram
{
    public:
        Telegram_Log(EventSystemParticipant* source, std::string log, EventSystem::LoggerAdapter::level_t level);
        Telegram_Log();
        virtual ~Telegram_Log();
        char* getLog();
        void setLog(std::string log, EventSystem::LoggerAdapter::level_t level);
        time_t getTime();
        char* getSourceID();
        char* getUniqueSourceID();
        EventSystem::LoggerAdapter::level_t getLevel();

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
        EventSystem::LoggerAdapter::level_t level;
        time_t logtime;
};

#endif // TELEGRAM_LOG_H
