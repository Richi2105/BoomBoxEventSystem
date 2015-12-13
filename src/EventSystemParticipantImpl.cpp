#include <pthread.h>
#include <unistd.h>

#include "../include/Telegram/Telegram.h"
#include "../include/Telegram/Telegram_Log.h"

#include "../include/EventSystemParticipantImpl.h"
#include "../include/Logging/LoggerAdapter.h"

void* checkForMessageD(void* eventSystemPart)
{
    EventSystemParticipantImpl* evp = (EventSystemParticipantImpl*) eventSystemPart;
    printf("EventSysPartImpl_Thread: %s\n", evp->getUniqueIdentifier().c_str());
    void* espData = evp->getMessageMemory();
    void* data = malloc(4096);
    while (true)
    {
        memset(data, 0, 4096);
        int bytes = evp->getSocket()->receive(data, 4096);

        printf("Anonymous Telegram with size %d\n", bytes);

        pthread_mutex_lock(evp->getMemoryMutex());
        memcpy(espData, data, bytes);
        evp->setMessageReceived(true);
        pthread_cond_signal(evp->getReceivedCondition());
        pthread_mutex_unlock(evp->getMemoryMutex());
        //TODO: wait for message fetch / discard
    }
    return ((void*) 0);
}

EventSystemParticipantImpl::EventSystemParticipantImpl(std::string id) : socket()
{
	this->init(id);
}

EventSystemParticipantImpl::EventSystemParticipantImpl(std::string id, in_port_t port) : socket(port)
{
	this->init(id);
}

void EventSystemParticipantImpl::init(std::string id)
{
	printf("building %s Participant...\n", id.c_str());

	this->id = id;
	this->messageMemory = malloc(4096);
	this->sendMemory = malloc(4096);
	int error;

	LoggerAdapter::initLoggerAdapter(this);

	pthread_mutex_init(&this->memoryMutex, NULL);
	pthread_cond_init(&this->messageReceived, NULL);

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
    pthread_mutex_destroy(&this->memoryMutex);
    pthread_cond_destroy(&this->messageReceived);
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

pthread_mutex_t* EventSystemParticipantImpl::getMemoryMutex()
{
	return &this->memoryMutex;
}

pthread_cond_t* EventSystemParticipantImpl::getReceivedCondition()
{
	return &this->messageReceived;
}

void EventSystemParticipantImpl::send(Telegram* telegram)
{
	memset(this->sendMemory, 0, 4096);
	int bytes = telegram->serialize(this->sendMemory);
    this->socket.send(this->sendMemory, bytes);
}

int EventSystemParticipantImpl::receive(void* data, bool nonblocking)
{
    if (this->newMessage)
    {
		pthread_mutex_lock(&this->memoryMutex);
		memcpy(data, this->messageMemory, 4096);
		this->newMessage = false;
		pthread_mutex_unlock(&this->memoryMutex);

		return 1;

    }
    else
    {
    	if (nonblocking)
    	{
    		return 0;
    	}
    	else
    	{
			pthread_mutex_lock(&this->memoryMutex);
			pthread_cond_wait(&this->messageReceived, &this->memoryMutex);
			memcpy(data, this->messageMemory, 4096);
			this->newMessage = false;
			pthread_mutex_unlock(&this->memoryMutex);

			return 1;
    	}
    }

}
