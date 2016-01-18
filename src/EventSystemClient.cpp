#include <pthread.h>
#include <unistd.h>
#include <malloc.h>

#include "../include/constants.h"

#include "../include/Telegram/Telegram.h"

#include "../include/Telegram/TelegramObject.h"
#include "../include/Register/RegisterNetwork.h"
#include "../include/Register/RegisterLocal.h"

#include "../include/EventSystemClient.h"
#include "../include/Logging/LoggerAdapter.h"

namespace EventSystem
{

void* checkForMessageD(void* eventSystemPart)
{
	EventSystemClient* evp = (EventSystemClient*) eventSystemPart;
    printf("EventSysPartImpl_Thread: %s\n", evp->getUniqueIdentifier().c_str());
    void* espData = evp->getMessageMemory();
    void* data = malloc(4096);
    while (true)
    {
        memset(data, 0, 4096);
        int bytes = evp->getSocket()->receive(data, DATASIZE);

        printf("Anonymous Telegram with size %d\n", bytes);

        pthread_mutex_lock(evp->getMemoryMutex());
        memcpy(espData, data, bytes);
        evp->setMessageReceived(true);
        evp->setReceivedBytes(bytes);
        pthread_cond_signal(evp->getReceivedCondition());
        pthread_cond_wait(evp->getFetchedCondition(), evp->getMemoryMutex());
        pthread_mutex_unlock(evp->getMemoryMutex());


        //TODO: wait for message fetch / discard
    }
    return ((void*) 0);
}

EventSystemClient::EventSystemClient(std::string id) : socket()
{
	this->init(id);
}

EventSystemClient::EventSystemClient(std::string id, in_port_t port, char* networkDevice) : socket(port, networkDevice)
{
	this->init(id);
}

void EventSystemClient::init(std::string id)
{
	printf("building %s Participant...\n", id.c_str());

	this->id = id;
	this->messageMemory = malloc(4096);
	this->sendMemory = malloc(4096);
	this->newMessage = false;
	this->receivedBytes = 0;

	LoggerAdapter::initLoggerAdapter(this);

	pthread_mutex_init(&this->memoryMutex, NULL);
	pthread_cond_init(&this->messageReceived, NULL);
	pthread_cond_init(&this->messageFetched, NULL);

	printf("Event System Participant successful created\n");
}

EventSystemClient::~EventSystemClient()
{
	this->disconnect();
	pthread_mutex_destroy(&this->memoryMutex);
    pthread_cond_destroy(&this->messageReceived);
}

void EventSystemClient::disconnect()
{
	Serializeable* unreg;
	if (this->socket.isLocal())
	{
		unreg = new Register_Local((SocketAddressLocal*)this->socket.getAddress(), this->id);
	}
	else
	{
		unreg = new Register_Network((SocketAddressNetwork*)this->socket.getAddress(), this->id);
	}
	Telegram_Object* regTelegram = new Telegram_Object("MASTER", unreg);
	regTelegram->setType(Telegram::UNREGISTER);
	this->send(regTelegram);
}

void EventSystemClient::startReceiving()
{
	int error = pthread_create(&(this->connectThreadID), NULL, checkForMessageD, this);
	if (error != 0)
	{
		fprintf(stderr, "Severe: could not start receive thread: %d\n", error);
	}
	else
	{
		printf("receive thread successful started\n");
	}
}

void EventSystemClient::stopReceiving()
{
	int error = pthread_cancel(this->connectThreadID);
	if (error != 0)
	{
		fprintf(stderr, "Severe: could not send cancel to receive thread: %d\n", error);
	}
	else
	{
		printf("receive thread successful signaled to cancel\n");
	}

	error = pthread_join(this->connectThreadID, NULL);
	if (error != 0)
	{
		fprintf(stderr, "Severe: could not join receive thread: %d\n", error);
	}
	else
	{
		printf("receive thread successful joined\n");
	}

	this->disconnect();
}


int EventSystemClient::connectToMaster()
{
	if (this->socket.isLocal())
	{
		Register_Local* reg = new Register_Local((SocketAddressLocal*)this->socket.getAddress(), this->id);
		Telegram_Object* regTelegram = new Telegram_Object("MASTER", reg);
		regTelegram->setType(Telegram::REGISTER);
		this->send(regTelegram);
		return 0;
	}
	else
		return -1;
}
int EventSystemClient::connectToMaster(sockaddr_in address)
{
	if (!this->socket.isLocal())
	{
		this->socket.setAddress(address, sizeof(sockaddr_in));
		Register_Network* reg = new Register_Network((SocketAddressNetwork*)this->socket.getAddress(), this->id);
		Telegram_Object* regTelegram = new Telegram_Object("MASTER", reg);
		regTelegram->setType(Telegram::REGISTER);
		this->send(regTelegram);
		return 0;
	}
	else
		return -1;
}
std::string EventSystemClient::getIdentifier()
{
    return this->id;
}
std::string EventSystemClient::getUniqueIdentifier()
{
    return (this->socket.getSocket()->getUniqueID() + "__" + this->id);
}
void EventSystemClient::setAddress(sockaddr_in address, socklen_t len)
{
	this->socket.setAddress(address, len);
}
SocketAddress* EventSystemClient::getAddress()
{
    return this->socket.getAddress();
}
SocketIO* EventSystemClient::getSocket()
{
    return this->socket.getSocket();
}
void* EventSystemClient::getMessageMemory()
{
    return this->messageMemory;
}

void EventSystemClient::setMessageReceived(bool newMessage)
{
    this->newMessage = newMessage;
}

void EventSystemClient::setReceivedBytes(unsigned int numOfBytes)
{
	this->receivedBytes = numOfBytes;
}

unsigned int EventSystemClient::getReceivedBytes()
{
	return this->receivedBytes;
}

pthread_mutex_t* EventSystemClient::getMemoryMutex()
{
	return &this->memoryMutex;
}

pthread_cond_t* EventSystemClient::getReceivedCondition()
{
	return &this->messageReceived;
}

pthread_cond_t* EventSystemClient::getFetchedCondition()
{
	return &this->messageFetched;
}

void EventSystemClient::send(Telegram* telegram)
{
	memset(this->sendMemory, 0, DATASIZE);
	int bytes = telegram->serialize(this->sendMemory);
    this->socket.send(this->sendMemory, bytes);
}

void EventSystemClient::send(void* data, int numOfBytes)
{
	this->socket.send(data, numOfBytes);
}

void EventSystemClient::log(Telegram_Object* log)
{
	this->send(log);
}

inline int copyReceiveMem(EventSystemClient* evp, void* data)
{
	int retVal;
	if (evp->getReceivedBytes() > malloc_usable_size(data))
	{
		fprintf(stderr, "Function receive: not enough space allocated, %d Bytes needed", evp->getReceivedBytes());
		retVal = -1;
	}
	else
	{
		memcpy(data, evp->getMessageMemory(), evp->getReceivedBytes());
		retVal = evp->getReceivedBytes();
	}
	evp->setMessageReceived(false);
	pthread_cond_signal(evp->getFetchedCondition());

	return retVal;
}

int EventSystemClient::receive(void* data, bool nonblocking)
{
	int retVal;
    if (this->newMessage)
    {
    	pthread_mutex_lock(this->getMemoryMutex());
		retVal = copyReceiveMem(this, data);
		pthread_mutex_unlock(this->getMemoryMutex());
    }
    else
    {
    	if (nonblocking)
    	{
    		retVal = 0;
    	}
    	else
    	{
    		pthread_mutex_lock(this->getMemoryMutex());
    		pthread_cond_wait(this->getReceivedCondition(), this->getMemoryMutex());
			retVal = copyReceiveMem(this, data);
			pthread_mutex_unlock(this->getMemoryMutex());
    	}
    }
    return retVal;
}

} /* namespace EventSystem */
