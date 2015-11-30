#include <pthread.h>
#include <unistd.h>

#include "../include/Telegram/Telegram.h"
#include "../include/Telegram/Telegram_Log.h"

#include "../include/EventSystemParticipantImpl.h"

void* checkForMessageD(void* eventSystemPart)
{
    EventSystemParticipantImpl* evp = (EventSystemParticipantImpl*) eventSystemPart;
    printf("EventSysPartImpl_Thread: %s\n", evp->getUniqueIdentifier().c_str());
    void* espData = evp->getMessageMemory();
    void* data = malloc(4096);
    while (true)
    {
    	sleep(1);
        memset(data, 0, 4096);
        int bytes = evp->getSocket()->receive(data, 4096);
        evp->setMessageReceived(true);
        memcpy(espData, data, 4096);
        printf("Anonymous Telegram with size %d\n", bytes);

//        Telegram* telegram = new Telegram(((Telegram_Log*)data)->getSourceID());
//        evp->getSocket()->send((void*)telegram, telegram->getSize());
        memcpy(espData, data, bytes);

    }
    return ((void*) 0);
}

EventSystemParticipantImpl::EventSystemParticipantImpl(std::string id) : socket()
{
    printf("building %s Participant...\n", id.c_str());

    this->id = id;
    this->messageMemory = malloc(4096);
    this->sendMemory = malloc(4096);
    int error;

    error = pthread_create(&(this->connectThreadID), NULL, checkForMessageD, this);
    printf("Error: %d\n", error);
    if (error < 0)
    {
        exit (-1);
    }
    else
    {
        printf("Event System Participant successful created\n");
    }
}

EventSystemParticipantImpl::EventSystemParticipantImpl(std::string id, in_port_t port) : socket(port)
{
    printf("building %s Participant...\n", id.c_str());

    this->id = id;
    this->messageMemory = malloc(4096);
    this->sendMemory = malloc(4096);
    int error;

    error = pthread_create(&(this->connectThreadID), NULL, checkForMessageD, this);
    printf("Error: %d\n", error);
    if (error < 0)
    {
        exit (-1);
    }
    else
    {
        printf("Event System Participant successful created\n");
    }
}

EventSystemParticipantImpl::~EventSystemParticipantImpl()
{
    //dtor
}
int EventSystemParticipantImpl::connectToMaster()
{
	if (this->socket.isLocal())
	{
		this->socket.connect(this->id);
		return 0;
	}
	else
		return -1;
}
int EventSystemParticipantImpl::connectToMaster(sockaddr_in address)
{
	if (!this->socket.isLocal())
	{
		this->socket.setAddress(address, sizeof(sockaddr_in));
		this->socket.connect(this->id);
		return 0;
	}
	else
		return -1;
}
std::string EventSystemParticipantImpl::getIdentifier()
{
    return this->id;
}
std::string EventSystemParticipantImpl::getUniqueIdentifier()
{
    return (this->socket.getSocket()->getUniqueID() + "__" + this->id);
}
void EventSystemParticipantImpl::setAddress(sockaddr_in address, socklen_t len)
{
	this->socket.setAddress(address, len);
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
	memset(this->sendMemory, 0, 4096);
	printf("EventSystemParticipantImpl::send(): sending to %s\n", telegram->getDestinationID());
	int bytes = telegram->serialize(this->sendMemory);
    this->socket.send(this->sendMemory, bytes);
}

int EventSystemParticipantImpl::receive(void* data, bool nonblocking)
{
    if (nonblocking)
    {
        if (this->newMessage)
        {
            memcpy(data, this->messageMemory, 4096);
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
