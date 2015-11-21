#ifndef TELEGRAM_LOG_H
#define TELEGRAM_LOG_H

#include <string>
#include <time.h>
#include <stdio.h>

#include "Telegram.h"
#include "../EventSystemParticipant.h"

#define LOG_MESSAGE_SIZE 250
#define UNIQUEID_SIZE 30

class Telegram_Log : public Telegram
{
    public:
        Telegram_Log(EventSystemParticipant* source, std::string log);
        virtual ~Telegram_Log();
        char* getLog();
        time_t getTime();
        char* getSourceID();
        char* getUniqueSourceID();
    protected:
    private:
/*        std::string log;
        std::string sourceID;
        std::string uniqueSourceID;*/
        char log[LOG_MESSAGE_SIZE];
        char sourceID[ID_SIZE];
        char uniqueSourceID[UNIQUEID_SIZE];
        time_t logtime;
};

#endif // TELEGRAM_LOG_H
