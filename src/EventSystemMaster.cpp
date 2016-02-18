#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>

#include <time.h>

#include "../include/constants.h"

#include "../include/SocketAddress.h"
#include "../include/Telegram/Telegram.h"

#include "../include/Logging/LoggerAdapter.h"
#include "../include/Logging/Log.h"

#include "../include/Telegram/TelegramObject.h"
#include "../include/Register/RegisterInterface.h"
#include "../include/Register/RegisterLocal.h"
#include "../include/Register/RegisterNetwork.h"

#include "../include/EventSystemMaster.h"
#include "../include/EventSystemParticipant.h"

#include "../include/StateMachine/StateObject.h"
#include "../include/StateMachine/StateMachine.h"

#include "../include/Watchdog/ClientWatchdog.h"


namespace EventSystem
{

inline bool stringCompare(const char* str1, const char* str2)
{
    if (strcmp(str1, str2) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

inline std::string cropID(std::string id)
{
	unsigned int pos = id.find_last_of('_') + 1;
	std::string retVal = id.substr(pos > id.size() ? 0 : pos, ID_SIZE);
	return retVal;
}

inline std::string cropUID(std::string id)
{
	unsigned int pos = id.find_first_of('_');
	std::string retVal = id.substr(0, pos > id.size() ? 0 : pos);
	return retVal;
}

inline int contains(ClientVector* vec, char* uid)
{
	int retVal = 0;
	for (ClientWatchdog* client : *vec)
	{
		if ( stringCompare(client->getClientAddress()->getUniqueID(), uid) )
		{
			printf("contains(): found!\n");
			return retVal;
		}
		retVal += 1;
	}
	retVal = -1;
	return retVal;
}

struct threadArgument {
	bool local;
	bool* run;
	EventSystemMaster* masterPointer;
	void* dataPointer;
};

const char id_master[] = "MASTER";
const char id_logger[] = "LOGGER";

inline void error_routine(int error, const char* message)
{
	if (error != 0)
		fprintf(stderr, "Error %d: %s\n", error, message);
}

void cleanup_data(void* data)
{
	#ifdef DEBUG_OUT
	printf("cleaning up data\n");
	#endif //DEBUG_OUT
	free(data);
}

void* getStatus(void* arg)
{
    EventSystemMaster* evm = ((threadArgument*)arg)->masterPointer;
    char* uid = (char*)((threadArgument*)arg)->dataPointer;
    bool* run = ((threadArgument*)arg)->run;

    pthread_cleanup_push(cleanup_data, uid);
    pthread_cleanup_push(cleanup_data, arg);

    while (*run)
    {
    	printf("getStatus Thread\n");
    	pthread_mutex_lock(&evm->pingMutex);
    	pthread_cond_wait(&evm->pingCondition, &evm->pingMutex);
    	memcpy(uid, evm->currUIDPingResponse, UNIQUEID_SIZE);
		if (evm->currStatusPingResponse->getState() > StateMachine::EVENTSYSTEM_STATE_CUSTOM)
		{
			std::string message = "Client ";
			message.append(uid);
			message.append(" has status ");
			message.append(evm->currStatusPingResponse->getCustomState());
			LoggerAdapter::log(Log::STATUS, message);
		}
		else
		{
			std::string message = "Client ";
			message.append(uid);
			message.append(" has status ");
			message.append(StateMachine::getDefaultStateDescription(evm->currStatusPingResponse->getState()));
			LoggerAdapter::log(Log::STATUS, message);
		}
    	pthread_mutex_unlock(&evm->pingMutex);
    	pthread_cond_signal(&evm->pingCondition);

    	ClientWatchdog* client = evm->getClientByUID(uid);
    	if (client == nullptr)
    	{
    		std::string message = "A client ";
    		message += uid;
    		message += " answered the ping signal but is not connected";
    		LoggerAdapter::log(Log::SEVERE, message);
    	}
    	else
    	{
    		client->responded();
    	}
    }

    free(uid);
    free(arg);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return (void*) 0;
}

void* checkStatus(void* arg)
{
    EventSystemMaster* evm = ((threadArgument*)arg)->masterPointer;
    void* data = ((threadArgument*)arg)->dataPointer;
    bool* run = ((threadArgument*)arg)->run;

    pthread_cleanup_push(cleanup_data, data);
    pthread_cleanup_push(cleanup_data, arg);

    Telegram pingTelegram("");
    pingTelegram.setType(Telegram::PING);
    pingTelegram.setSource(evm->getUniqueIdentifier());
    pingTelegram.serialize(data);

//    struct timespec waitTime;
//    int timedwait_retVal;

    while (*run)
    {
    	printf("Checking status\n");
    	pthread_mutex_lock(&evm->clientMutex);
    	ClientMap* cmap = new ClientMap(*(evm->clients));
    	pthread_mutex_unlock(&evm->clientMutex);

    	for (ClientPair pair : *cmap)
    	{
    		for (ClientWatchdog* c : *(pair.second))
    		{
    			if ( (c->didRespondInTime()) )
    			{
    				printf("true\n");
    				evm->master.send(data, pingTelegram.getSerializedSize(), c->getClientAddress());
    			}
    			else
    			{
    				printf("removing %s\n", c->getClientAddress()->getUniqueID());
    				std::string message = "Client ";
    				message.append(c->getClientAddress()->getUniqueID());
    				message.append(" timeout");
    				LoggerAdapter::log(Log::WARNING, message);
    				evm->removeClient(c->getClientAddress()->getUniqueID(), c->getClientAddress());
    			}
    		}
    	}
    	delete cmap;
		sleep(5);
    }
    free(data);
    free(arg);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return (void*) 0;
}

void* checkForMessage(void* arg)
{
    EventSystemMaster* evm = ((threadArgument*)arg)->masterPointer;
    bool local = ((threadArgument*)arg)->local;
    bool* run = ((threadArgument*)arg)->run;
    void* data = ((threadArgument*)arg)->dataPointer;
    int bytes;

    pthread_cleanup_push(cleanup_data, data);
    pthread_cleanup_push(cleanup_data, arg);

    SocketIO* socket;
    if (local)
    {
    	socket = evm->getLocalSocket();
    }
    else
    {
    	socket = evm->getNetworkSocket();
    }

    Telegram telegram("");

    while (*run)
    {
        memset(data, 0, DATASIZE);
        bytes = socket->receive(data, DATASIZE);
        telegram.deserialize(data);

#ifdef DEBUG_OUT
        unsigned char* data1 = (unsigned char*) ((threadArgument*)arg)->dataPointer;
		printf("Contents of Telegram:\n");
		for (int i=0; i<bytes; i+=1)
		{
			if ((i) % 8==0)
			{
				printf("\t(%d:) ", i);
			}

			printf("%2x ", *(data1+i));

			if ((i+1) % 16==0)
			{
				printf("\n");
			}

		}
		printf("\n");
#endif //DEBUG_OUT

		if (stringCompare(cropID(telegram.getDestinationID()).c_str(), Telegram::ID_MASTER.c_str()))
		{
			switch (telegram.getType()) {
			case TELEGRAM_REGISTER: {
				Telegram_Object* regTelegram = new Telegram_Object();
				if (local)
				{
					EventSystem::Register_Local* reg = new EventSystem::Register_Local();
					regTelegram->deserialize(data, reg);
					std::string s = reg->getClientID();

					evm->addClient(s, (SocketAddress*)reg->getClientAddress());
					delete reg;
				}
				else
				{
					EventSystem::Register_Network* reg = new EventSystem::Register_Network();
					regTelegram->deserialize(data, reg);
					std::string s = reg->getClientID();

					evm->addClient(s, (SocketAddress*)reg->getClientAddress());
					delete reg;
				}
				delete regTelegram;
				break;
			}
			case TELEGRAM_UNREGISTER: {
				Telegram_Object* regTelegram = new Telegram_Object();
				if (local)
				{
					EventSystem::Register_Local* reg = new EventSystem::Register_Local();
					regTelegram->deserialize(data, reg);
					std::string s = reg->getClientID();

					evm->removeClient(regTelegram->getSourceID(), (SocketAddress*)reg->getClientAddress());
					//evm->removeClient(contains(&evm->allClients, reg->getClientAddress()->getUniqueID()));
					//evm->removeClient(s, reg->getClientAddress());
					//LoggerAdapter::log(Log::INFO, "Client " + s + " unregistered");
					delete reg;
				}
				else
				{
					EventSystem::Register_Network* reg = new EventSystem::Register_Network();
					regTelegram->deserialize(data, reg);
					std::string s = reg->getClientID();

					evm->removeClient(regTelegram->getSourceID(), (SocketAddress*)reg->getClientAddress());
					//evm->removeClient(contains(&evm->allClients, reg->getClientAddress()->getUniqueID()));
					//evm->removeClient(s, reg->getClientAddress());
					//LoggerAdapter::log(Log::INFO, "Client " + s + " unregistered");
					delete reg;
				}
				delete regTelegram;
				break;
			}
			case TELEGRAM_PING: {
				Telegram_Object* statusTelegram = new Telegram_Object();
				pthread_mutex_lock(&evm->pingMutex);
				statusTelegram->deserialize(data, evm->currStatusPingResponse);
				memcpy(evm->currUIDPingResponse, telegram.getSourceID(), UNIQUEID_SIZE);
				pthread_cond_signal(&evm->pingCondition);
				struct timespec t;
				clock_gettime(CLOCK_REALTIME, &t);
				t.tv_nsec += 500;
				pthread_cond_timedwait(&evm->pingCondition, &evm->pingMutex, &t);
				pthread_mutex_unlock(&evm->pingMutex);
				//pthread_yield();

				delete statusTelegram;
				break;
			}
			default: {
				LoggerAdapter::log(Log::WARNING, "No action for x");
				break;
			}
			}
		}
		else
		{
			printf("Sending to %s, isUniqueID: %s\n", telegram.getDestinationID(), telegram.isUniqueDestination() ? "true" : "false");
			evm->sendToClient(telegram.getDestinationID(), telegram.isUniqueDestination(), data, bytes);
		}

    }

    free(data);
    free(arg);
    pthread_cleanup_pop(0);
    pthread_cleanup_pop(0);
    return (void*) 0;
}

EventSystemMaster::EventSystemMaster() : master(PORT)
{
	this->init();
}

EventSystemMaster::EventSystemMaster(char* networkDevice) : master(networkDevice, PORT)
{
	this->init();
}

void EventSystemMaster::init()
{
    printf("EventSystemMaster(): building Master...\n");
    this->master.broadcast();
    this->id = "MASTER";

    this->nLoggerConnected = 0;

    int error;

    this->dataPointer = malloc(512);

    this->currUIDPingResponse = (char*) malloc(UNIQUEID_SIZE);

    this->currStatusPingResponse = new StateObject();

    ClientWatchdog::setResponseTime(15);

    pthread_mutex_init(&this->clientMutex, NULL);
    pthread_mutex_init(&this->pingMutex, NULL);
    pthread_cond_init(&this->pingCondition, NULL);


    this->run_localMessageThread = true;
    this->run_networkMessageThread = true;
    this->run_checkStatusThread = true;
    this->run_getStatusThread = true;

    this->clients = new ClientMap();

    struct threadArgument* arg_local = (threadArgument*) malloc(sizeof(threadArgument));

    arg_local->local = true;
    arg_local->run = &this->run_localMessageThread;
    arg_local->masterPointer = this;
    arg_local->dataPointer = malloc(DATASIZE);

    error = pthread_create(&this->local_messageThreadID, NULL, checkForMessage, arg_local);

    if (error < 0)
	{
		exit (-1);
	}
    printf("EventSystemMaster(): Local message thread started\n");

    struct threadArgument* arg_network = (threadArgument*) malloc(sizeof(threadArgument));

    arg_network->local = false;
    arg_network->run = &this->run_networkMessageThread;
    arg_network->masterPointer = this;
    arg_network->dataPointer = malloc(DATASIZE);

    error = pthread_create(&this->network_messageThreadID, NULL, checkForMessage, arg_network);

    if (error < 0)
    {
        exit (-1);
    }
	printf("EventSystemMaster(): Network message thread started\n");
    struct threadArgument* arg_check = (threadArgument*) malloc(sizeof(threadArgument));

    arg_check->local = false;
    arg_check->run = &this->run_checkStatusThread;
    arg_check->masterPointer = this;
    arg_check->dataPointer = malloc(128);

    error = pthread_create(&this->checkStatusID, NULL, checkStatus, arg_check);

    if (error < 0)
    {
        exit (-1);
    }
	printf("EventSystemMaster(): check status thread started\n");

    struct threadArgument* arg_get = (threadArgument*) malloc(sizeof(threadArgument));

    arg_get->local = false;
    arg_get->run = &this->run_getStatusThread;
    arg_get->masterPointer = this;
    arg_get->dataPointer = malloc(64);

    error = pthread_create(&this->getStatusID, NULL, getStatus, arg_get);

    if (error < 0)
    {
        exit (-1);
    }
	printf("EventSystemMaster(): get status thread started\n");

    //sleep(1);
	printf("EventSystemMaster(): Event System Master successful created\n");

}

EventSystemMaster::~EventSystemMaster()
{
	printf("Deleting Master\n");

	pthread_mutex_destroy(&this->clientMutex);
    pthread_mutex_destroy(&this->pingMutex);
    pthread_cond_destroy(&this->pingCondition);

    int error;

    this->run_localMessageThread = false;
    error = pthread_cancel(this->local_messageThreadID);
    error_routine(error, "could not cancel local_messageThread");
    error = pthread_join(this->local_messageThreadID, NULL);
    error_routine(error, "could not join local_messageThread");

    this->run_networkMessageThread = false;
    error = pthread_cancel(this->network_messageThreadID);
    error_routine(error, "could not cancel network_messageThread");
    error = pthread_join(this->network_messageThreadID, NULL);
    error_routine(error, "could not join network_messageThread");

    this->run_checkStatusThread = false;
    error = pthread_cancel(this->checkStatusID);
    error_routine(error, "could not cancel checkStatusThread");
    error = pthread_join(this->checkStatusID, NULL);
    error_routine(error, "could not join checkStatusThread");

    this->run_getStatusThread = false;
    error = pthread_cancel(this->getStatusID);
    error_routine(error, "could not cancel getStatusThread");
    error = pthread_join(this->getStatusID, NULL);
    error_routine(error, "could not join getStatusThread");


    free(this->currUIDPingResponse);
    delete this->currStatusPingResponse;

    Telegram* quitTelegram = new Telegram();
    quitTelegram->setSource(this->getUniqueIdentifier());
    quitTelegram->setType(Telegram::QUIT);
    int bytes = quitTelegram->serialize(this->dataPointer);
	for (ClientPair res : *(this->clients))
	{
		for (ClientWatchdog* c : *res.second)
		{
			this->sendToClient(c->getClientAddress()->getUniqueID(), true, this->dataPointer, bytes);
			delete c;
		}
		delete res.second;
	}
	delete this->clients;
	delete quitTelegram;

	free(this->dataPointer);

    printf("Master deleted\n");
}

std::string EventSystemMaster::getIdentifier()
{
    return this->id;
}
std::string EventSystemMaster::getUniqueIdentifier()
{
    return (this->master.getUniqueID() + "__" + this->id);
}
SocketIO* EventSystemMaster::getSocket()
{
    return (SocketIO*)this->master.getLocalSocket();
}
SocketIO_Local* EventSystemMaster::getLocalSocket()
{
    return (SocketIO_Local*)this->master.getLocalSocket();
}
SocketIO_Network* EventSystemMaster::getNetworkSocket()
{
    return (SocketIO_Network*)this->master.getNetworkSocket();
}
SocketAddress* EventSystemMaster::getAddress()
{
	return this->master.getNetworkSocket()->getAddress();
}

void EventSystemMaster::addClient(std::string id, SocketAddress* address)
{
	pthread_mutex_lock(&this->clientMutex);
	SocketAddress* newAddress;
	if (address->isLocal())
		newAddress = new SocketAddressLocal(*((SocketAddressLocal*) address) );
	else
		newAddress = new SocketAddressNetwork(*((SocketAddressNetwork*) address) );

	ClientMap::iterator result = this->clients->find(id);
	if (result == this->clients->end())
	{
		ClientVector* vec = new ClientVector();
		ClientWatchdog* cwd = new ClientWatchdog(newAddress);
		vec->push_back(cwd);
		ClientPair entry(id, vec);
		this->clients->insert(entry);
	}
	else
	{
		ClientWatchdog* cwd = new ClientWatchdog(newAddress);
		result->second->push_back(cwd);
	}
    if (stringCompare(id.c_str(), id_logger))
    {
    	this->inkrementConnectedLogger();
    }
	pthread_mutex_unlock(&this->clientMutex);
	std::string uid = address->getUniqueID();
	LoggerAdapter::log(Log::INFO, "Client " + uid + " registered");
}

void EventSystemMaster::removeClient(std::string uid, SocketAddress* address)
{
	bool error = true;
	pthread_mutex_lock(&this->clientMutex);
	ClientMap::const_iterator result = this->clients->find(cropID(uid));
	if (result == this->clients->end())
	{
		//error
	}
	else
	{
		unsigned int i = 0;
		for (ClientWatchdog* client : *(result->second))
		{
			if ( stringCompare( client->getClientAddress()->getUniqueID(), uid.c_str() ) )
			{
				delete client;
				result->second->erase(result->second->begin() + i);
				if (stringCompare(cropID(uid).c_str(), id_logger))
					this->dekrementConnectedLogger();
				error = false;
				break;
			}
			i += 1;
		}
	}
	pthread_mutex_unlock(&this->clientMutex);
	if (error)
	{
		std::string logmessage = "client " + uid + " tried to unregister, but is not registered";
		LoggerAdapter::log(Log::WARNING, logmessage);
	}
	else
	{
		std::string logmessage = "client " + uid + " unregistered successful";
		LoggerAdapter::log(Log::INFO, logmessage);
	}
}


ClientWatchdog* EventSystemMaster::getClientByUID(std::string uid)
{
	ClientWatchdog* clientResult = nullptr;
	pthread_mutex_lock(&this->clientMutex);
	ClientMap::const_iterator result = this->clients->find(cropID(uid));
	if (result == this->clients->end())
	{
		//error
	}
	else
	{
		for (ClientWatchdog* client : *(result->second))
		{
			if ( stringCompare( client->getClientAddress()->getUniqueID(), uid.c_str() ) )
			{
				clientResult = client;
				break;
			}
		}
	}
	pthread_mutex_unlock(&this->clientMutex);
	return clientResult;
}

void EventSystemMaster::sendToClient(std::string destination, bool isUniqueID, void* data, int numOfBytes)
{
/*	if (stringCompare(cropID(destination).c_str(), Telegram::ID_LOGGER.c_str()) && !this->loggerConnected)
	{
		Telegram_Object* logTelegram = new Telegram_Object();
		Log* logMessage = new Log();
		logTelegram->deserialize(data, logMessage);
		this->log(logTelegram);
	}
	else
*/
	bool error = true;
	pthread_mutex_lock(&this->clientMutex);
	ClientMap::const_iterator result = this->clients->find(cropID(destination));
	if (result == this->clients->end())
	{
		//error
	}
	else
	{
		for (ClientWatchdog* client : *(result->second))
		{

			if (isUniqueID)
			{
				if (stringCompare(client->getClientAddress()->getUniqueID(), cropUID(destination).c_str()))
				{
					this->master.send(data, numOfBytes, client->getClientAddress());
					error = false;
					break;
				}
			}
			else
			{
				error = false;
				this->master.send(data, numOfBytes, client->getClientAddress());
			}
		}
	}
	pthread_mutex_unlock(&this->clientMutex);

	if (error)
	{
		LoggerAdapter::log(Log::WARNING, "no such client " + destination + " connected yet");
	}
}

void EventSystemMaster::log(Telegram_Object* log)
{
	if (this->nLoggerConnected > 0)
	{
		int bytes = log->serialize(this->dataPointer);
		this->sendToClient("LOGGER", false, this->dataPointer, bytes);
	}
	else
	{
		EventSystem::Log* logMessage = (EventSystem::Log*) log->getObject();
		time_t currTime = logMessage->getTime();
		printf("On %s: Message: %s, with %d bytes from %s\n", ctime(&currTime), logMessage->getLog(), logMessage->getSerializedSize(), logMessage->getUniqueSourceID());
	}
}

void EventSystemMaster::inkrementConnectedLogger()
{
	this->nLoggerConnected += 1;
}
void EventSystemMaster::dekrementConnectedLogger()
{
	if (this->nLoggerConnected > 0)
		this->nLoggerConnected -= 1;
}

} /* namespace EventSystem */
