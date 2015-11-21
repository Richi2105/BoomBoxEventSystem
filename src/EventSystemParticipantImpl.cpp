#include <pthread.h>
#include <unistd.h>

#include "../include/Telegram/Telegram.h"
#include "../include/Telegram/Telegram_Log.h"

#include "../include/EventSystemParticipantImpl.h"

void* checkForMessageD(void* eventSystemPart)
{
    EventSystemParticipantImpl* evp = (EventSystemParticipantImpl*) eventSystemPart;
    printf("%s\n", evp->getUniqueIdentifier().c_str());
    Telegram* data = (Telegram*)evp->getMessageMemory();
    while (true)
    {
        memset(data, 0, 4096);
        evp->getSocket()->receive((void*)data, 4096);
        evp->setMessageReceived(true);
        printf("Anonymous Telegram with size %d\n", data->getSize());

        Telegram* telegram = new Telegram(((Telegram_Log*)data)->getSourceID());
        evp->getSocket()->send((void*)telegram, telegram->getSize());

    }
    return ((void*) 0);
}

EventSystemParticipantImpl::EventSystemParticipantImpl(std::string id)
{
    printf("building %s Participant...\n", id.c_str());

    this->id = id;
    this->messageMemory = malloc(4096);
    this->socket.connect(this->id);
    int error;

    error = pthread_create(&(this->connectThreadID), NULL, checkForMessageD, this);
    printf("Error: %d\n", error);
    if (error < 0)
    {
        exit (-1);
    }
    else
    {
        sleep(1);
        printf("Event System Participant successful created\n");
    }
}

EventSystemParticipantImpl::~EventSystemParticipantImpl()
{
    //dtor
}

std::string EventSystemParticipantImpl::getIdentifier()
{
    return this->id;
}
std::string EventSystemParticipantImpl::getUniqueIdentifier()
{
    return (this->socket.getUniqueID() + "__" + this->id);
}
SocketAddress* EventSystemParticipantImpl::getAddress()
{
    return this->socket.getAddress();
}
SocketIO* EventSystemParticipantImpl::getSocket()
{
    return this->socket.getSocket();
}
void* EventSystemParticipantImpl::getMessageMemory()
{
    return this->messageMemory;
}

void EventSystemParticipantImpl::setMessageReceived(bool newMessage)
{
    this->newMessage = newMessage;
}

void EventSystemParticipantImpl::send(Telegram* telegram)
{
    this->socket.send((void*) telegram, telegram->getSize());
}

int EventSystemParticipantImpl::receive(Telegram* telegram, bool nonblocking)
{
    if (nonblocking)
    {
        if (this->newMessage)
        {
            memcpy(telegram, this->messageMemory, 4096);
            this->newMessage = false;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        //todo
        return -1;
    }

}