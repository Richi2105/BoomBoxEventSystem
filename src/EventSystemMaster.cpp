#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>

#include <time.h>

#include "../include/constants.h"

#include "../include/SocketAddress.h"
#include "../include/Telegram/Telegram.h"
#include "../include/Telegram/Telegram_Log.h"
#include "../include/Telegram/Telegram_Register.h"
#include "../include/Telegram/Telegram_Register_Extern.h"

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

bool stringCompare(const char* str1, const char* str2)
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

    printf("checkForMessage(): %s thread starting\n", local ? "local" : "network");
    printf("\tdata information:\n\tbool local: %p\n\tvoid* data: %p\n\tint bytes: %p\n", &local, &data, &bytes);

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

#ifdef DEBUG
        unsigned char* data1 = (unsigned char*) data;
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
#endif //DEBUG

		if (stringCompare(telegram.getDestinationID(), id_master))
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
					LoggerAdapter::log(LoggerAdapter::INFO, "Client " + s + " registered");
				}
				else if (regTelegram->getType() == Telegram::Telegram::UNREGISTER)
				{
					evm->removeClient(s, reg->getClientAddress());
					LoggerAdapter::log(LoggerAdapter::INFO, "Client " + s + " unregistered");
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
					LoggerAdapter::log(LoggerAdapter::INFO, "Client " + s + " registered");
				}
				else if (regTelegram->getType() == Telegram::Telegram::UNREGISTER)
				{
					evm->removeClient(s, reg->getClientAddress());
					LoggerAdapter::log(LoggerAdapter::INFO, "Client " + s + " unregistered");
				}

			}

		}
		else
		{
			evm->sendToClient(telegram.getDestinationID(), data, bytes);
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

    struct threadArgument arg_local, arg_network;

    arg_local.local = true;
    arg_local.masterPointer = this;
    arg_local.dataPointer = malloc(DATASIZE);

    arg_network.local = false;
    arg_network.masterPointer = this;
    arg_network.dataPointer = malloc(DATASIZE);

    this->dataPointer = malloc(512);

    LoggerAdapter::initLoggerAdapter(this);

    error = pthread_create(&connectThreadID, NULL, checkForMessage, &arg_local);
    if (error < 0)
	{
		exit (-1);
	}
    printf("EventSystemMaster(): Local message thread started\n");
    sleep(2);
    error = pthread_create(&connectThreadNetworkID, NULL, checkForMessage, &arg_network);
    printf("Error: %d\n", error);
    if (error < 0)
    {
        exit (-1);
    }
    else
    {
    	printf("EventSystemMaster(): Network message thread started\n");
    	printf("EventSystemMaster(): Event System Master successful created\n");
    }
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
    	LoggerAdapter::log(LoggerAdapter::WARNING, "client " + id + " tried to unregister, but is not registered\n");
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
    		LoggerAdapter::log(LoggerAdapter::WARNING, "client " + id + " tried to unregister, but is not registered\n");
    	}
    }
}

void EventSystemMaster::removeClient(std::string id, SocketAddressNetwork* remAddress)
{
	NetworkClientMap::const_iterator result = this->networkClients.find(id);
    if (result == this->networkClients.end())
    {
    	LoggerAdapter::log(LoggerAdapter::WARNING, "client " + id + " tried to unregister, but is not registered\n");
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
    		LoggerAdapter::log(LoggerAdapter::WARNING, "client " + id + " tried to unregister, but is not registered\n");
    	}
    }
}

void EventSystemMaster::sendToClient(std::string destination, void* data, int numOfBytes)
{
	if (stringCompare(destination.c_str(), id_logger) && !this->loggerConnected)
	{
		Telegram_Object* logTelegram = new Telegram_Object();
		Log* logMessage = new Log();
		logTelegram->deserialize(data, logMessage);
		this->log(logTelegram);
	}
	else
	{
		int noClient = 0;
		LocalClientMap::const_iterator result = this->localClients.find(destination);
		if (result == this->localClients.end())
		{
			noClient += 1;
		}
		else
		{
			for (SocketAddressLocal address : result->second)
			{
				this->master.send(data, numOfBytes, &address);
			}
		}
		NetworkClientMap::const_iterator result2 = this->networkClients.find(destination);
		if (result2 == this->networkClients.end())
		{
			noClient += 1;
		}
		else
		{
			for (SocketAddressNetwork address : result2->second)
			{
				this->master.send((void*)data, numOfBytes, &address);
			}
		}

		if (noClient >= 2)
		{
			LoggerAdapter::log(LoggerAdapter::WARNING, "no client " + destination + " connected yet");
		}
	}

}

void EventSystemMaster::log(Telegram_Object* log)
{
	if (this->loggerConnected)
	{
		int bytes = log->serialize(this->dataPointer);
		this->sendToClient("LOGGER", this->dataPointer, bytes);
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
