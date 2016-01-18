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

struct threadArgument {
	bool local;
	EventSystemMaster* masterPointer;
	void* dataPointer;
};

const char id_master[] = "MASTER";
const char id_logger[] = "LOGGER";

void* checkForMessage(void* arg)
{
    EventSystemMaster* evm = ((threadArgument*)arg)->masterPointer;
    bool local = ((threadArgument*)arg)->local;
    void* data = ((threadArgument*)arg)->dataPointer;
    int bytes;

//    printf("bool: %d\n", ((threadArgument*)arg)->local);

//    printf("checkForMessage(): %s thread starting\n", ((threadArgument*)arg)->local ? "local" : "network");
//    printf("\tdata information:\n\tbool local: %p\n\tvoid* data: %p\n\tint bytes: %p\n", &((threadArgument*)arg)->local, ((threadArgument*)arg)->dataPointer, &bytes);

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

    while (true)
    {
        memset(data, 0, DATASIZE);
        bytes = socket->receive(data, DATASIZE);
        telegram.deserialize(data);

#ifdef DEBUG_OUT
        unsigned char* data1 = (unsigned char*) ((threadArgument*)arg)->dataPointer;
		printf("Contents of Telegram:\n");
		for (int i=0; i<bytes; i+=1)
		{
			printf("%2x ", *(data1+i));
			if (i%16==0)
			{
				printf("\n");
			}

		}
		printf("\n");
#endif //DEBUG_OUT

		if (stringCompare(telegram.getDestinationID(), Telegram::ID_MASTER.c_str()))
		{
			Telegram_Object* regTelegram = new Telegram_Object();
			std::string s;
			if (local)
			{
				EventSystem::Register_Local* reg = new EventSystem::Register_Local();
				regTelegram->deserialize(data, reg);
				s = reg->getClientID();
				if (regTelegram->getType() == Telegram::Telegram::REGISTER)
				{
					evm->addClient(s, reg->getClientAddress());
					LoggerAdapter::log(Log::INFO, "Client " + s + " registered");
				}
				else if (regTelegram->getType() == Telegram::Telegram::UNREGISTER)
				{
					evm->removeClient(s, reg->getClientAddress());
					LoggerAdapter::log(Log::INFO, "Client " + s + " unregistered");
				}
			}
			else
			{
				EventSystem::Register_Network* reg = new EventSystem::Register_Network();
				regTelegram->deserialize(data, reg);
				s = reg->getClientID();
				if (regTelegram->getType() == Telegram::Telegram::REGISTER)
				{
					evm->addClient(s, reg->getClientAddress());
					LoggerAdapter::log(Log::INFO, "Client " + s + " registered");
				}
				else if (regTelegram->getType() == Telegram::Telegram::UNREGISTER)
				{
					evm->removeClient(s, reg->getClientAddress());
					LoggerAdapter::log(Log::INFO, "Client " + s + " unregistered");
				}
			}

		}
		else
		{
			printf("Sending to %s, isUniqueID: %s\n", telegram.getDestinationID(), telegram.isUniqueDestination() ? "true" : "false");
			evm->sendToClient(telegram.getDestinationID(), telegram.isUniqueDestination(), data, bytes);
		}

    }
    return ((void*) 0);
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

    this->loggerConnected = false;

    int error;
    pthread_t connectThreadID;
    pthread_t connectThreadNetworkID;

    this->dataPointer = malloc(512);

    LoggerAdapter::initLoggerAdapter(this);

    struct threadArgument* arg_local = (threadArgument*) malloc(sizeof(threadArgument));

    arg_local->local = true;
    arg_local->masterPointer = this;
    arg_local->dataPointer = malloc(DATASIZE);

    error = pthread_create(&connectThreadID, NULL, checkForMessage, arg_local);

    if (error < 0)
	{
		exit (-1);
	}
    printf("EventSystemMaster(): Local message thread started\n");

    struct threadArgument* arg_network = (threadArgument*) malloc(sizeof(threadArgument));

    arg_network->local = false;
    arg_network->masterPointer = this;
    arg_network->dataPointer = malloc(DATASIZE);

    error = pthread_create(&connectThreadNetworkID, NULL, checkForMessage, arg_network);

    if (error < 0)
    {
        exit (-1);
    }

	printf("EventSystemMaster(): Network message thread started\n");

    //sleep(1);
	printf("EventSystemMaster(): Event System Master successful created\n");

}

EventSystemMaster::~EventSystemMaster()
{
    //dtor
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

void EventSystemMaster::addClient(std::string id, SocketAddressLocal* address)
{
//	SocketAddressLocal locAddress(address->address, address->len);
	LocalClientMap::const_iterator result = this->localClients.find(id);
    if (result == this->localClients.end())
    {
    	LocalClientVector vec;
        vec.push_back(*address);
        std::pair<std::string, LocalClientVector> entry (id, vec);
        this->localClients.insert(entry);
    }
    else
    {
        (this->localClients.at(id)).push_back(*address);
    }

    if (stringCompare(id.c_str(), id_logger))
    {
    	this->loggerConnected = true;
    }
}

void EventSystemMaster::addClient(std::string id, SocketAddressNetwork* address)
{
//	SocketAddressNetwork netAddress(address->address, address->len);
	NetworkClientMap::const_iterator result = this->networkClients.find(id);
    if (result == this->networkClients.end())
    {
    	NetworkClientVector vec;
        vec.push_back(*address);
        std::pair<std::string, NetworkClientVector> entry (id, vec);
        this->networkClients.insert(entry);
    }
    else
    {
        (this->networkClients.at(id)).push_back(*address);
    }

    if (stringCompare(id.c_str(), id_logger))
    {
    	this->loggerConnected = true;
    }
}

void EventSystemMaster::removeClient(std::string id, SocketAddressLocal* remAddress)
{
	LocalClientMap::const_iterator result = this->localClients.find(id);
    if (result == this->localClients.end())
    {
    	LoggerAdapter::log(Log::WARNING, "client " + id + " tried to unregister, but is not registered\n");
    }
    else
    {
    	unsigned int i = 0;
    	for (SocketAddressLocal add : this->localClients.at(id))
		{
    		if (add == remAddress)
    		{
    			this->localClients.at(id).erase(this->localClients.at(id).begin() + i);
    			break;
    		}
    		i += 1;
		}
    	if (i >= this->localClients.at(id).size())
    	{
    		LoggerAdapter::log(Log::WARNING, "client " + id + " tried to unregister, but is not registered\n");
    	}
    }
}

void EventSystemMaster::removeClient(std::string id, SocketAddressNetwork* remAddress)
{
	NetworkClientMap::const_iterator result = this->networkClients.find(id);
    if (result == this->networkClients.end())
    {
    	LoggerAdapter::log(Log::WARNING, "client " + id + " tried to unregister, but is not registered\n");
    }
    else
    {
    	unsigned int i = 0;
    	for (SocketAddressNetwork add : this->networkClients.at(id))
		{
    		if (add == remAddress)
    		{
    			this->networkClients.at(id).erase(this->networkClients.at(id).begin() + i);
    			break;
    		}
    		i += 1;
		}
    	if (i >= this->networkClients.at(id).size())
    	{
    		LoggerAdapter::log(Log::WARNING, "client " + id + " tried to unregister, but is not registered\n");
    	}
    }
}

void EventSystemMaster::sendToClient(std::string destination, bool isUniqueID, void* data, int numOfBytes)
{
	if (stringCompare(cropID(destination).c_str(), Telegram::ID_LOGGER.c_str()) && !this->loggerConnected)
	{
		Telegram_Object* logTelegram = new Telegram_Object();
		Log* logMessage = new Log();
		logTelegram->deserialize(data, logMessage);
		this->log(logTelegram);
	}
	else
	{
		int noClient = 0;
		LocalClientMap::const_iterator result = this->localClients.find(cropID(destination));
		if (result == this->localClients.end())
		{
			noClient += 1;
		}
		else
		{
			for (SocketAddressLocal address : result->second)
			{
				if (isUniqueID)
				{
					printf("Local: %s\n", address.getUniqueID());
					if (stringCompare(address.getUniqueID(), cropUID(destination).c_str()))
					{
						this->master.send(data, numOfBytes, &address);
					}
				}
				else
				{
					this->master.send(data, numOfBytes, &address);
				}
			}
		}

		NetworkClientMap::const_iterator result2 = this->networkClients.find(cropID(destination));
		if (result2 == this->networkClients.end())
		{
			noClient += 1;
		}
		else
		{
			for (SocketAddressNetwork address : result2->second)
			{
				if (isUniqueID)
				{
					printf("Network: %s\n", address.getUniqueID());
					if (stringCompare(address.getUniqueID(), cropUID(destination).c_str()))
					{
						this->master.send(data, numOfBytes, &address);
					}
				}
				else
					this->master.send(data, numOfBytes, &address);
			}
		}


		if (noClient >= 2)
		{
			LoggerAdapter::log(Log::WARNING, "no client " + cropID(destination) + " connected yet");
		}
	}

}

void EventSystemMaster::log(Telegram_Object* log)
{
	if (this->loggerConnected)
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

void EventSystemMaster::setLoggerConnected()
{
	this->loggerConnected = true;
}
bool EventSystemMaster::isLoggerConnected()
{
	return this->loggerConnected;
}

} /* namespace EventSystem */
