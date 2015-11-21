#include "../include/Telegram/Telegram_Log.h"

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
