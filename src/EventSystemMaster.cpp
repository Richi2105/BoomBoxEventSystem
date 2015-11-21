#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>

#include <time.h>

#include "../include/SocketAddress.h"
#include "../include/Telegram/Telegram.h"
#include "../include/Telegram/Telegram_Log.h"
#include "../include/Telegram/Telegram_Register.h"

#include "../include/EventSystemMaster.h"

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

const char id_master[] = "MASTER";
const char id_logger[] = "LOGGER";

void* checkForMessageLocal(void* eventSystemMaster)
{

    EventSystemMaster* evm = (EventSystemMaster*) eventSystemMaster;
    printf("%s\n", evm->getUniqueIdentifier().c_str());
    Telegram* data = (Telegram*)malloc(4096);
    printf("Pointer to data: %p\n", data);

    time_t currTime = time(NULL);
    while (true)
    {
        memset(data, 0, 4096);
        int bytes = evm->getSocket()->receive((void*)data, 4096);
        printf("Destination: %s\nReceived Bytes: %d\n", data->getDestinationID(), bytes);


        if (stringCompare(data->getDestinationID(), id_master))
        {
            printf("Client trys to register: %s\nTelegram size: %d\n", ((Telegram_Register*)data)->getClientID(), data->getSize());

            std::string s(((Telegram_Register*)data)->getClientID());
            evm->addClient(s, *((Telegram_Register*)data)->getClientAddress());
        }
        else if (stringCompare(data->getDestinationID(), id_logger))
        {
            currTime = ((Telegram_Log*)data)->getTime();
            printf("On %s: Message: %s, with %d bytes from %s\n", ctime(&currTime), ((Telegram_Log*)data)->getLog(), data->getSize(), ((Telegram_Log*)data)->getSourceID());
            evm->sendToClient(data->getDestinationID(), data);
        }
        else
        {
            evm->sendToClient(data->getDestinationID(), data);
        }



    }
    return ((void*) 0);
}

EventSystemMaster::EventSystemMaster() : master(666)
{
//    this->master = *(new SocketReaderMaster());
    printf("building Master...\n");
    this->master.broadcast();
    this->id = "MASTER";

    int error;
    pthread_t connectThreadID;
    error = pthread_create(&connectThreadID, NULL, checkForMessageLocal, this);
//    this->threadCheckMessageLocal = new std::thread(checkForMessageLocal);
    printf("Error: %d\n", error);
    if (error < 0)
    {
        exit (-1);
    }
    else
    {
        sleep(1);
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
    return nullptr;
}

void* EventSystemMaster::getDataPointer()
{
    return dataPointer;
}

void* EventSystemMaster::checkForMessage(void* arg)
{
	Telegram* data = (Telegram*)malloc(4096);
	printf("Pointer to data: %p\n", data);

	time_t currTime = time(NULL);
	while (true)
	{
		memset(data, 0, 4096);
		int bytes = this->getSocket()->receive((void*)data, 4096);
		printf("Destination: %s\nReceived Bytes: %d\n", data->getDestinationID(), bytes);


		if (stringCompare(data->getDestinationID(), id_master))
		{
			printf("Client trys to register: %s\nTelegram size: %d\n", ((Telegram_Register*)data)->getClientID(), data->getSize());

			std::string s(((Telegram_Register*)data)->getClientID());
			this->addClient(s, *((Telegram_Register*)data)->getClientAddress());
		}
		else
		{
			this->sendToClient(data->getDestinationID(), data);
		}



	}
	return ((void*) 0);

}

void EventSystemMaster::addClient(std::string id, SocketAddressLocal address)
{
	LocalClientMap::const_iterator result = this->localClients.find(id);
    if (result == this->localClients.end())
    {
    	LocalClientVector vec;
        vec.push_back(address);
        std::pair<std::string, LocalClientVector> entry (id, vec);
        this->localClients.insert(entry);
    }
    else
    {
        (this->localClients.at(id)).push_back(address);
    }
}

void EventSystemMaster::addClient(std::string id, SocketAddressNetwork address)
{
	NetworkClientMap::const_iterator result = this->networkClients.find(id);
    if (result == this->networkClients.end())
    {
    	NetworkClientVector vec;
        vec.push_back(address);
        std::pair<std::string, NetworkClientVector> entry (id, vec);
        this->networkClients.insert(entry);
    }
    else
    {
        (this->networkClients.at(id)).push_back(address);
    }
}

void EventSystemMaster::sendToClient(std::string destination, Telegram* data)
{
    LocalClientMap::const_iterator result = this->localClients.find(destination);
    if (result == this->localClients.end())
    {
        ;
    }
    else
    {
        for (SocketAddressLocal address : result->second)
        {
            this->master.send((void*)data, data->getSize(), &address);
        }
    }
    NetworkClientMap::const_iterator result2 = this->networkClients.find(destination);
	if (result2 == this->networkClients.end())
	{
		;
	}
	else
	{
		for (SocketAddressNetwork address : result2->second)
		{
			this->master.send((void*)data, data->getSize(), &address);
		}
	}

}
