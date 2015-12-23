#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>

#include <time.h>

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

void* checkForMessageLocal(void* eventSystemMaster)
{

    EventSystemMaster* evm = (EventSystemMaster*) eventSystemMaster;
    printf("%s\n", evm->getUniqueIdentifier().c_str());
    void* data = malloc(4096);
    Telegram::Telegram* telegram = new Telegram::Telegram("");
    Telegram_Register telegram_reg;
    printf("Local Receive Thread: Pointer to data: %p\n", data);

    time_t currTime = time(NULL);
    while (true)
    {
    	sleep(1);
        memset(data, 0, 4096);
        int bytes = evm->getLocalSocket()->receive(data, 4096);
        printf("Local Receive Thread: received something\n");

        telegram->deserialize(data);
        printf("Local Receive Thread:\nDestination: %s\nReceived Bytes: %d\n", telegram->getDestinationID(), bytes);

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

        if (stringCompare(telegram->getDestinationID(), id_master))
        {
        	telegram_reg.deserialize(data);
            printf("Local Receive Thread:\nClient tries to register: %s\n", telegram_reg.getClientID());

            std::string s(telegram_reg.getClientID());
            evm->addClient(s, telegram_reg.getClientAddress());
        }
        else if (stringCompare(telegram->getDestinationID(), id_logger))
        {
        	Telegram_Log log;
        	log.deserialize(data);
            currTime = log.getTime();
            printf("Local Receive Thread:\nOn %s: Message: %s, with %d bytes from %s\n", ctime(&currTime), log.getLog(), log.getSize(), log.getUniqueSourceID());
            evm->sendToClient(telegram->getDestinationID(), data, bytes);
        }
        else
        {
            evm->sendToClient(telegram->getDestinationID(), data, bytes);
        }



    }
    return ((void*) 0);
}

void* checkForMessageNetwork(void* eventSystemMaster)
{

    EventSystemMaster* evm = (EventSystemMaster*) eventSystemMaster;
    printf("%s\n", evm->getUniqueIdentifier().c_str());
    void* data = malloc(4096);
    Telegram::Telegram telegram("");
/*    printf("For your information: %p Telegram\n"
    		"\t\t%p destinationIDTelegram\n"
    		"offset: %d\n", data, &data->destinationID, (unsigned char)((Telegram*)(&data->destinationID) - data));
*/
    printf("Network receive Thread: Pointer to data: %p\n", data);

    time_t currTime = time(NULL);
    while (true)
    {
    	sleep(1);
        memset(data, 0, 4096);
        int bytes = evm->getNetworkSocket()->receive(data, 4096);
        printf("Network Receive Thread: received something\n");

        telegram.deserialize(data);
        printf("Destination: %s\nReceived Bytes: %d\n", telegram.getDestinationID(), bytes);

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
        	Telegram_Register_Extern telegram_reg;
        	telegram_reg.deserialize(data);
            printf("Network receive Thread:\nClient tries to register: %s\n", telegram_reg.getClientID());

            std::string s(telegram_reg.getClientID());
            evm->addClient(s, telegram_reg.getClientAddress());
        }
        else if (stringCompare(telegram.getDestinationID(), id_logger))
        {
        	Telegram_Log log;
        	log.deserialize(data);
            currTime = log.getTime();
            printf("Network receive Thread:\nOn %s: Message: %s, with %d bytes from %s\n", ctime(&currTime), log.getLog(), log.getSize(), log.getUniqueSourceID());
            evm->sendToClient(telegram.getDestinationID(), data, bytes);
        }
        else
        {
            evm->sendToClient(telegram.getDestinationID(), data, bytes);
        }


    }
    return ((void*) 0);
}

void* checkForMessage(void* arg)
{
    EventSystemMaster* evm = ((threadArgument*)arg)->masterPointer;
    bool local = ((threadArgument*)arg)->local;
    void* data = ((threadArgument*)arg)->dataPointer;
    int bytes;

    printf("%s thread starting\n", local ? "local" : "network");
    printf("data information:\bool local: %p\nvoid* data: %p\nint bytes: %p\n", &local, &data, &bytes);

    SocketIO* socket;
    if (local)
    {
    	socket = evm->getLocalSocket();
    }
    else
    {
    	socket = evm->getNetworkSocket();
    }

    Telegram::Telegram telegram("");

    while (true)
    {
        memset(data, 0, 4096);
        bytes = socket->receive(data, 4096);
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
			Telegram::Telegram_Object* regTelegram = new Telegram::Telegram_Object();
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

/*		switch (telegram.getType())
		{
	case Telegram::Telegram::REGISTER: {
			Telegram::Telegram_Object* regTelegram = new Telegram::Telegram_Object();
			std::string s;
			if (local)
			{
				EventSystem::Register_Local* reg = new EventSystem::Register_Local();
				regTelegram->deserialize(data, reg);
				s = reg->getClientID();
				evm->addClient(s, reg->getClientAddress());
			}
			else
			{
				EventSystem::Register_Network* reg = new EventSystem::Register_Network();
				regTelegram->deserialize(data, reg);
				evm->addClient(s, reg->getClientAddress());
			}

			if (stringCompare(s.c_str(), id_logger))
			{
				evm->setLoggerConnected();
			}

			if (stringCompare(telegram.getDestinationID(), id_master))
			{
				LoggerAdapter::log(LoggerAdapter::INFO, "Client " + s + " registered");
			}
			else
			{
				LoggerAdapter::log(LoggerAdapter::WARNING, "Client " + s + " registered, but addressed wrong!");
			}
			break;
		}

	case Telegram::Telegram::UNREGISTER: {

			break;
		}
	default: {
			evm->sendToClient(telegram.getDestinationID(), data, bytes); break;
		}
		}
*/
/*
        if (stringCompare(telegram.getDestinationID(), id_master))
        {
        	std::string s;
        	if (local)
        	{
        		Telegram_Register* telegram_reg = new Telegram_Register();
        		telegram_reg->deserialize(data);
        		s = (telegram_reg->getClientID());
        		evm->addClient(s, telegram_reg->getClientAddress());
        	}
        	else
        	{
        		Telegram_Register_Extern* telegram_reg = new Telegram_Register_Extern();
        		telegram_reg->deserialize(data);
        		s = (telegram_reg->getClientID());
        		evm->addClient(s, telegram_reg->getClientAddress());
        	}
            printf("Receive Thread:\nClient tries to register: %s\n", s.c_str());
            if (stringCompare(s.c_str(), id_logger))
            {
            	evm->setLoggerConnected();
            }

        }
        else if (stringCompare(telegram.getDestinationID(), id_logger))
        {
        	if (evm->isLoggerConnected())
        	{
        		evm->sendToClient(telegram.getDestinationID(), data, bytes);
        	}
        	Telegram_Log log;
        	log.deserialize(data);
            currTime = log.getTime();
            printf("Receive Thread:\nOn %s: Message: %s, with %d bytes from %s\n", ctime(&currTime), log.getLog(), log.getSize(), log.getUniqueSourceID());
        }
        else
        {
            evm->sendToClient(telegram.getDestinationID(), data, bytes);
        }
*/

    }
    return ((void*) 0);
}

EventSystemMaster::EventSystemMaster() : master(666)
{
//    this->master = *(new SocketReaderMaster());
    printf("building Master...\n");
    this->master.broadcast();
    this->id = "MASTER";

    this->loggerConnected = false;

    int error;
    pthread_t connectThreadID;
    pthread_t connectThreadNetworkID;

    struct threadArgument arg_local, arg_network;

    arg_local.local = true;
    arg_local.masterPointer = this;
    arg_local.dataPointer = malloc(4096);

    arg_network.local = false;
    arg_network.masterPointer = this;
    arg_network.dataPointer = malloc(4096);

    this->dataPointer = malloc(512);

    LoggerAdapter::initLoggerAdapter(this);

    error = pthread_create(&connectThreadID, NULL, checkForMessage, &arg_local);
    if (error < 0)
	{
		exit (-1);
	}
    printf("Local message thread started\n");
    sleep(2);
    error = pthread_create(&connectThreadNetworkID, NULL, checkForMessage, &arg_network);
    printf("Error: %d\n", error);
    if (error < 0)
    {
        exit (-1);
    }
    else
    {
    	printf("Network message thread started\n");
    	printf("Event System Master successful created\n");
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
		Telegram::Telegram_Object* logTelegram = new Telegram::Telegram_Object();
		EventSystem::Log* logMessage = new EventSystem::Log();
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

void EventSystemMaster::log(Telegram::Telegram_Object* log)
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
