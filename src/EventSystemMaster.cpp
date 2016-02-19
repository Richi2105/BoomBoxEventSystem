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

#include "../include/Watchdog/ClientWatchdogV2.h"


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
	#ifdef DEBUG_OUT
	printf("in cropID(%s): retVal = %s\n", id.c_str(), retVal.c_str());
	#endif //DEBUG_OUT
	return retVal;
}

inline std::string cropUID(std::string id)
{
	unsigned int pos = id.find_first_of('_');
	std::string retVal = id.substr(0, pos > id.size() ? 0 : pos);
	#ifdef DEBUG_OUT
	printf("in cropUID(%s): retVal = %s\n", id.c_str(), retVal.c_str());
	#endif //DEBUG_OUT
	return retVal;
}

inline int contains(ClientVector* vec, char* uid)
{
	int retVal = 0;
	for (ClientWatchdogV2* client : *vec)
	{
		if ( stringCompare(client->getClientAddress()->getUniqueID(), uid) )
		{
			#ifdef DEBUG_OUT
			printf("in contains(vec, %s): retVal = %d\n", uid, retVal);
			#endif //DEBUG_OUT
			return retVal;
		}
		retVal += 1;
	}
	retVal = -1;
	#ifdef DEBUG_OUT
	printf("in contains(vec, %s): retVal = %d\n", uid, retVal);
	#endif //DEBUG_OUT
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

    pthread_cleanup_push(cleanup_data, arg);
    pthread_cleanup_push(cleanup_data, uid);

    while (*run)
    {
		#ifdef DEBUG_OUT
		printf("getStatus()\n");
		#endif //DEBUG_OUT
    	pthread_mutex_lock(&evm->pingMutex);
    	pthread_cond_wait(&evm->pingCondition, &evm->pingMutex);
    	int responseNrs = evm->getResponseNrs();
    	pthread_mutex_unlock(&evm->pingMutex);


    	while (responseNrs != 0)
    	{
    		pthread_mutex_lock(&evm->pingMutex);
        	EventSystemMaster::pingResponse* pR = evm->popResponse();
        	responseNrs = evm->getResponseNrs();
        	pthread_mutex_unlock(&evm->pingMutex);

        	if (pR != nullptr)
        	{
    	//    	memcpy(uid, evm->currUIDPingResponse, UNIQUEID_SIZE);
    			if (pR->state->getState() > StateMachine::EVENTSYSTEM_STATE_CUSTOM)
    			{
    				std::string message = "Client ";
    				message.append(pR->uid);
    				message.append(" has status ");
    				message.append(pR->state->getCustomState());
    				LoggerAdapter::log(Log::STATUS, message);
    			}
    			else
    			{
    				std::string message = "Client ";
    				message.append(pR->uid);
    				message.append(" has status ");
    				message.append(StateMachine::getDefaultStateDescription(pR->state->getState()));
    				LoggerAdapter::log(Log::STATUS, message);
    			}
    			delete pR->state;

    	//    	pthread_cond_signal(&evm->pingCondition);

    			ClientWatchdogV2* client = evm->getClientByUID(pR->uid);
    			if (client == nullptr)
    			{
    				std::string message = "A client ";
    				message += pR->uid;
    				message += " answered the ping signal but is not connected";
    				LoggerAdapter::log(Log::SEVERE, message);
    			}
    			else
    			{
    				client->set();
    				#ifdef DEBUG_OUT
    				printf("getStatus(): client %s responded", pR->uid);
    				#endif //DEBUG_OUT
    			}
    			free(pR->uid);
    			free(pR);

        	}
    	}
    }

    free(arg);
    free(uid);
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
		#ifdef DEBUG_OUT
		printf("Checking status\n");
		#endif //DEBUG_OUT
    	pthread_mutex_lock(&evm->clientMutex);
    	ClientMap* cmap = new ClientMap(*(evm->clients));
    	pthread_mutex_unlock(&evm->clientMutex);

    	for (ClientPair pair : *cmap)
    	{
    		for (ClientWatchdogV2* c : *(pair.second))
    		{
    			if (c != nullptr)
    			{
					if ( !(c->isTriggered()) )
					{
						#ifdef DEBUG_OUT
						printf("in checkStatus(): client %s has time left\n", c->getClientAddress()->getUniqueID());
						#endif //DEBUG_OUT
						evm->master.send(data, pingTelegram.getSerializedSize(), c->getClientAddress());
					}
					else
					{
						#ifdef DEBUG_OUT
						printf("in checkStatus(): client %s did not respond in time\n", c->getClientAddress()->getUniqueID());
						#endif //DEBUG_OUT
						std::string message = "Client ";
						message.append(c->getClientAddress()->getUniqueID());
						message.append(" timeout");
						LoggerAdapter::log(Log::WARNING, message);
						evm->removeClient(c->getClientAddress()->getUniqueID(), c->getClientAddress());
					}
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
        printf("checkForMessage()_%s: received from %s to %s\n"
        		"----------------\n", local ? "local" : "network", telegram.getSourceID(), telegram.getDestinationID());
#endif //DEBUG_OUT

#ifdef DEBUG_TELEGRAM_OUT
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
#endif //DEBUG_TELEGRAM_OUT

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
				EventSystemMaster::pingResponse* pR = (EventSystemMaster::pingResponse*) malloc(sizeof(EventSystemMaster::pingResponse));
				pR->state = new StateObject();
				pR->uid = (char*)malloc(UNIQUEID_SIZE);
				statusTelegram->deserialize(data, pR->state);
				memcpy(pR->uid, statusTelegram->getSourceID(), UNIQUEID_SIZE);
				pthread_mutex_lock(&evm->pingMutex);
				evm->pushResponse(pR);
				pthread_cond_signal(&evm->pingCondition);
//				struct timespec t;
//				clock_gettime(CLOCK_REALTIME, &t);
//				t.tv_nsec += 500;
//				pthread_cond_timedwait(&evm->pingCondition, &evm->pingMutex, &t);
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
			#ifdef DEBUG_OUT
			printf("Sending to %s, isUniqueID: %s\n", telegram.getDestinationID(), telegram.isUniqueDestination() ? "true" : "false");
			#endif //DEBUG_OUT
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

	this->responseNrs = 0;

    this->master.broadcast();
    this->id = "MASTER";

    this->nLoggerConnected = 0;

    int error;

    this->dataPointer = malloc(512);

    this->currUIDPingResponse = (char*) malloc(UNIQUEID_SIZE);

    this->currStatusPingResponse = new StateObject();

//    ClientWatchdogV2::setResponseTime(15);

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
		for (ClientWatchdogV2* c : *res.second)
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
		ClientWatchdogV2* cwd = new ClientWatchdogV2(newAddress, 15, 0);
		cwd->set();
		vec->push_back(cwd);
		ClientPair entry(id, vec);
		this->clients->insert(entry);
	}
	else
	{
		ClientWatchdogV2* cwd = new ClientWatchdogV2(newAddress, 15, 0);
		cwd->set();
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
		for (ClientWatchdogV2* client : *(result->second))
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


ClientWatchdogV2* EventSystemMaster::getClientByUID(std::string uid)
{
	ClientWatchdogV2* clientResult = nullptr;
	pthread_mutex_lock(&this->clientMutex);
	ClientMap::const_iterator result = this->clients->find(cropID(uid));
	if (result == this->clients->end())
	{
		//error
	}
	else
	{
		for (ClientWatchdogV2* client : *(result->second))
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
		for (ClientWatchdogV2* client : *(result->second))
		{

			if (isUniqueID)
			{
				if (stringCompare(client->getClientAddress()->getUniqueID(), destination.c_str()))
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

EventSystemMaster::pingResponse* EventSystemMaster::popResponse()
{
	printf("in EventSystemMaster::popResponse: responseNr %d\n", this->responseNrs-1);
	if (this->responseNrs > 0)
	{
		this->responseNrs -= 1;
		return this->responseStack[this->responseNrs];
	}
	else
		return nullptr;
}
void EventSystemMaster::pushResponse(EventSystemMaster::pingResponse* response)
{
	this->responseStack[this->responseNrs] = response;
	this->responseNrs += 1;
	printf("in EventSystemMaster::pushResponse: responseNr %d\n", this->responseNrs);
	if (this->responseNrs >= 20)
		fprintf(stderr, "Warning: Response Stack overflow\n");
}

int EventSystemMaster::getResponseNrs()
{
	return this->responseNrs;
}

} /* namespace EventSystem */
