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

#include "../include/StateMachine/StateObject.h"

namespace EventSystem
{

struct threadArgument {
	EventSystemClient* esc;
	bool* run;
};

inline void error_routine(int error, const char* message)
{
	if (error != 0)
		fprintf(stderr, "Error %d: %s\n", error, message);
}

void unlock_mutex(void* mutex)
{
	#ifdef DEBUG_OUT
	printf("unlocking mutex\n");
	#endif //DEBUG_OUT
	pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

void cleanup_data(void* data)
{
	#ifdef DEBUG_OUT
	printf("cleaning up data\n");
	#endif //DEBUG_OUT
	free(data);
}

void* checkWatchdog(void* arg)
{
	EventSystemClient* evp = ((threadArgument*)arg)->esc;
	bool* run = ((threadArgument*)arg)->run;
	pthread_cleanup_push(cleanup_data, arg);
	while (*run)
	{
		sleep(3);
		if (evp->watchdog->isTriggered())
		{
			evp->setConnectionError();
			break;
		}
	}
	pthread_cleanup_pop(0);
	return (void*) 0;
}

void* checkForMessageD(void* arg)
{
	EventSystemClient* evp = ((threadArgument*)arg)->esc;
	bool* run = ((threadArgument*)arg)->run;
	bool stop = false;
    printf("EventSysPartImpl_Thread: %s\n", evp->getUniqueIdentifier().c_str());
    void* espData = evp->getMessageMemory();
    void* data = malloc(4096);

    pthread_cleanup_push(cleanup_data, data);
    pthread_cleanup_push(cleanup_data, arg);
    pthread_cleanup_push(unlock_mutex, evp->getMemoryMutex());

    Telegram t("");
    while (!stop)
    {
        memset(data, 0, DATASIZE);
        int bytes = evp->getSocketSlave()->receive(data, DATASIZE);
        evp->watchdog->set();

		#ifdef DEBUG_OUT
		printf("Anonymous Telegram with size %d\n", bytes);
		#endif //DEBUG_OUT

		//peak into data
		t.deserialize(data);
		printf("Telegram type: %d\n", t.getType());
		if (t.getType() == TELEGRAM_PING)
		{
			#ifdef DEBUG_OUT
			printf("ping request received\n");
			#endif //DEBUG_OUT
			StateObject* so = new StateObject(evp->statemachine->getState(), evp->statemachine->getStateDescription());
			Telegram_Object* objTelegram = new Telegram_Object(t.getSourceID(), so);
			objTelegram->setSource(evp->getUniqueIdentifier());
//			objTelegram->setIdentifier(Telegram::ID_MASTER);
			objTelegram->setType(Telegram::PING);
			evp->send(objTelegram);
			delete so;
			delete objTelegram;
			pthread_mutex_lock(evp->getMemoryMutex());
			if (!(*run))
				stop = true;
			pthread_mutex_unlock(evp->getMemoryMutex());
		}
		else
		{
			pthread_mutex_lock(evp->getMemoryMutex());
			memcpy(espData, data, bytes);
			evp->setMessageReceived(true);
			evp->setReceivedBytes(bytes);
			pthread_cond_signal(evp->getReceivedCondition());
			pthread_cond_wait(evp->getFetchedCondition(), evp->getMemoryMutex());
			pthread_mutex_unlock(evp->getMemoryMutex());
		}
    }
    free(data);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return (void*) 0;
}

EventSystemClient::EventSystemClient(std::string id) : socket()
{
	this->statemachine = new StateMachine();
	this->statemachine->setState(StateMachine::EVENTSYSTEM_STATE_IDLE);

	this->watchdog = new Watchdog(10, 500);

	printf("building %s Participant...\n", id.c_str());

	this->id = id;
	this->messageMemory = malloc(4096);
	this->sendMemory = malloc(4096);
	this->newMessage = false;
	this->receivedBytes = 0;

	this->run_MessageThread = false;
	this->run_WatchdogThread = false;

	this->watchdogThreadID = 0;
	this->messageThreadID = 0;

	this->isStopped = true;
	this->connected = false;
	this->connectionError = false;

//	LoggerAdapter::initLoggerAdapter(this);

	pthread_mutex_init(&this->memoryMutex, NULL);
	pthread_cond_init(&this->messageReceived, NULL);
	pthread_cond_init(&this->messageFetched, NULL);

	printf("Event System Participant successful created\n");
}

EventSystemClient::EventSystemClient(std::string id, in_port_t port, char* networkDevice) : socket(port, networkDevice)
{
	this->statemachine = new StateMachine();
	this->statemachine->setState(StateMachine::EVENTSYSTEM_STATE_IDLE);

	this->watchdog = new Watchdog(12, 500);

	printf("building %s Participant...\n", id.c_str());

	this->id = id;
	this->messageMemory = malloc(4096);
	this->sendMemory = malloc(4096);
	this->newMessage = false;
	this->receivedBytes = 0;

	this->run_MessageThread = false;
	this->run_WatchdogThread = false;

	this->watchdogThreadID = 0;
	this->messageThreadID = 0;

	this->isStopped = true;
	this->connected = false;
	this->connectionError = false;

//	LoggerAdapter::initLoggerAdapter(this);

	pthread_mutex_init(&this->memoryMutex, NULL);
	pthread_cond_init(&this->messageReceived, NULL);
	pthread_cond_init(&this->messageFetched, NULL);

	printf("Event System Participant successful created\n");
}

EventSystemClient::~EventSystemClient()
{
	if (!isStopped)
		this->stopReceiving(false);

	pthread_mutex_destroy(&this->memoryMutex);
    pthread_cond_destroy(&this->messageReceived);
    pthread_cond_destroy(&this->messageFetched);
    free(this->messageMemory);
    free(this->sendMemory);
    delete this->statemachine;
    delete this->watchdog;
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
	regTelegram->setSource(this->getUniqueIdentifier());
	this->send(regTelegram);
	this->connected = false;
	delete unreg;
	delete regTelegram;
}

void EventSystemClient::startReceiving()
{
	if (!this->connectionError)
	{
		this->watchdog->set();
		this->run_MessageThread = true;
		this->isStopped = false;
		struct threadArgument* ta_message = (struct threadArgument*) malloc(sizeof(struct threadArgument));
		ta_message->esc = this;
		ta_message->run = &this->run_MessageThread;
		int error = pthread_create(&(this->messageThreadID), NULL, checkForMessageD, ta_message);
		if (error != 0)
		{
			fprintf(stderr, "Severe: could not start receive thread: %d\n", error);
		}
		else
		{
			printf("receive thread successful started\n");
		}

		this->run_WatchdogThread = true;
		struct threadArgument* ta_watchdog = (struct threadArgument*) malloc(sizeof(struct threadArgument));
		ta_watchdog->esc = this;
		ta_watchdog->run = &this->run_WatchdogThread;
		error = pthread_create(&(this->watchdogThreadID), NULL, checkWatchdog, ta_watchdog);
		if (error != 0)
		{
			fprintf(stderr, "Severe: could not start watchdog thread: %d\n", error);
		}
		else
		{
			printf("watchdog thread successful started\n");
		}
	}
}

void EventSystemClient::stopReceiving(bool softStop)
{
	if (!isStopped)
	{
		if (softStop)
		{
			pthread_mutex_lock(&this->memoryMutex);
			this->run_MessageThread = false;
			pthread_mutex_unlock(&this->memoryMutex);

		}
		else
		{
			int error = pthread_cancel(this->messageThreadID);
			if (error != 0)
			{
				fprintf(stderr, "Severe: could not send cancel to receive thread: %d\n", error);
			}
			else
			{
				printf("receive thread successful signaled to cancel\n");
			}
		}
		struct timespec time;
		clock_gettime(CLOCK_REALTIME, &time);
		time.tv_sec += 10;
		int error = pthread_timedjoin_np(this->messageThreadID, NULL, &time);
		if (error != 0)
		{
			fprintf(stderr, "Severe: could not join receive thread: %d\n", error);
			if (error == EBUSY)
				error = pthread_cancel(this->messageThreadID);
		}
		else
		{
			printf("receive thread successful joined\n");
		}

		this->run_WatchdogThread = false;
		clock_gettime(CLOCK_REALTIME, &time);
		time.tv_sec += 10;
		error = pthread_timedjoin_np(this->watchdogThreadID, NULL, &time);
		if (error != 0)
		{
			fprintf(stderr, "Severe: could not join watchdog thread: %d\n", error);
			if (error == EBUSY)
				error = pthread_cancel(this->watchdogThreadID);
		}
		else
		{
			printf("watchdog thread successful joined\n");
		}
		this->isStopped = true;
		this->disconnect();
	}
}


int EventSystemClient::connectToMaster()
{
	if (this->socket.isLocal())
	{
		Register_Local* reg = new Register_Local((SocketAddressLocal*)this->socket.getAddress(), this->id);
		reg->getClientAddress()->setUID(this->getUniqueIdentifier());
		Telegram_Object* regTelegram = new Telegram_Object("MASTER", reg);
		regTelegram->setType(Telegram::REGISTER);
		regTelegram->setSource(this->getUniqueIdentifier());
		this->send(regTelegram);
		this->connected = true;
		delete reg;
		delete regTelegram;
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
		reg->getClientAddress()->setUID(this->getUniqueIdentifier());
		Telegram_Object* regTelegram = new Telegram_Object("MASTER", reg);
		regTelegram->setType(Telegram::REGISTER);
		regTelegram->setSource(this->getUniqueIdentifier());
		this->send(regTelegram);
		this->connected = true;
		delete reg;
		delete regTelegram;
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
Socket_Slave* EventSystemClient::getSocketSlave()
{
	return &this->socket;
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

bool EventSystemClient::isConnected()
{
	return this->connected;
}

bool EventSystemClient::isConnectionError()
{
	return this->connectionError;
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

	return retVal;
}

int EventSystemClient::receive(void* data, int waitTime)
{
	int retVal;
	if (this->isStopped || this->connectionError)
		retVal = -1;
	else
	{
		this->statemachine->setState(StateMachine::EVENTSYSTEM_STATE_RECEIVING);
		if (this->newMessage)
		{
	pthread_mutex_lock(this->getMemoryMutex());
			retVal = copyReceiveMem(this, data);
			pthread_cond_signal(this->getFetchedCondition());
	pthread_mutex_unlock(this->getMemoryMutex());
		}
		else
		{
			if (waitTime == -1)
			{
				retVal = 0;
			}
			else
			{

				if (waitTime != 0)
				{

					struct timespec time;
					if (clock_gettime(CLOCK_REALTIME, &time) != 0)
						return -1;
					time.tv_sec += waitTime;
					printf("EventSystemClient::receive: waiting %d sec (@ %ld)\n", waitTime, time.tv_sec);
			pthread_mutex_lock(this->getMemoryMutex());
					int error = pthread_cond_timedwait(this->getReceivedCondition(), this->getMemoryMutex(), &time);
					printf("Error = %d\n", error);
					if (this->newMessage)
					{
						retVal = copyReceiveMem(this, data);
						pthread_cond_signal(this->getFetchedCondition());
					}
					else
						retVal = 0;
			pthread_mutex_unlock(this->getMemoryMutex());
				}
				else
				{
					printf("EventSystemClient::receive: waiting...\n");
			pthread_mutex_lock(this->getMemoryMutex());
					int error = pthread_cond_wait(this->getReceivedCondition(), this->getMemoryMutex());
					printf("Error = %d\n", error);
					retVal = copyReceiveMem(this, data);
					pthread_cond_signal(this->getFetchedCondition());
			pthread_mutex_unlock(this->getMemoryMutex());
				}

			}
		}
		this->statemachine->setState(StateMachine::EVENTSYSTEM_STATE_IDLE);
	}
    return retVal;
}

void EventSystemClient::setConnectionError()
{
	this->connectionError = true;
	this->connected = false;
	int error = pthread_mutex_trylock(&this->memoryMutex);
	if (error != 0)
	{
		fprintf(stderr, "memoryMutex could not be locked: deadlock\n");
	}
	else
	{
		printf("got memoryMutex\n");
		this->setReceivedBytes(0);
		pthread_cond_signal(this->getReceivedCondition());
		pthread_mutex_unlock(&this->memoryMutex);
	}
	this->stopReceiving(false);
}

} /* namespace EventSystem */
