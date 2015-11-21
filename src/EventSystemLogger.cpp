#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include "../include/Telegram/Telegram.h"
#include "../include/Telegram/Telegram_Log.h"

#include "../include/EventSystemLogger.h"

void* log(void* eventSystemPart)
{
    EventSystemParticipant* evp = (EventSystemParticipant*) eventSystemPart;
    printf("%s\n", evp->getUniqueIdentifier().c_str());
    time_t currTime;
    Telegram* data = (Telegram*)malloc(4096);
    while (true)
    {
        memset(data, 0, 4096);
        evp->getSocket()->receive((void*)data, 4096);
        currTime = ((Telegram_Log*)data)->getTime();
        printf("On %s: Message: %s, from %s\n", ctime(&currTime), ((Telegram_Log*)data)->getLog(), ((Telegram_Log*)data)->getUniqueSourceID());

        Telegram* telegram = new Telegram(((Telegram_Log*)data)->getSourceID());
        evp->getSocket()->send((void*)telegram, telegram->getSize());

    }
    return ((void*) 0);
}

EventSystemLogger::EventSystemLogger()
{
    printf("building Logger...\n");

    this->id = "LOGGER";
    this->slave.connect(this->id);
    int error;
    pthread_t connectThreadID;
    error = pthread_create(&connectThreadID, NULL, log, this);
    printf("Error: %d\n", error);
    if (error < 0)
    {
        exit (-1);
    }
    else
    {
        sleep(1);
        printf("Event System Logger successful created\n");
    }
}

EventSystemLogger::~EventSystemLogger()
{
    //dtor
}

std::string EventSystemLogger::getIdentifier()
{
    return this->id;
}
std::string EventSystemLogger::getUniqueIdentifier()
{
    return (this->slave.getUniqueID() + "__" + this->id);
}
SocketAddress* EventSystemLogger::getAddress()
{
    return this->slave.getAddress();
}

SocketIO* EventSystemLogger::getSocket()
{
	return this->slave.getSocket();
}
