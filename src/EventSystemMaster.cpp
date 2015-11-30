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

const char id_master[] = "MASTER";
const char id_logger[] = "LOGGER";

void* checkForMessageLocal(void* eventSystemMaster)
{

    EventSystemMaster* evm = (EventSystemMaster*) eventSystemMaster;
    printf("%s\n", evm->getUniqueIdentifier().c_str());
    void* data = malloc(4096);
    Telegram* telegram = new Telegram("");
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
    Telegram telegram("");
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

EventSystemMaster::EventSystemMaster() : master(666)
{
//    this->master = *(new SocketReaderMaster());
    printf("building Master...\n");
    this->master.broadcast();
    this->id = "MASTER";

    int error;
    pthread_t connectThreadID;
    pthread_t connectThreadNetworkID;
    error = pthread_create(&connectThreadID, NULL, checkForMessageLocal, this);
    if (error < 0)
	{
		exit (-1);
	}
    error = pthread_create(&connectThreadNetworkID, NULL, checkForMessageNetwork, this);
//    this->threadCheckMessageLocal = new std::thread(checkForMessageLocal);
    printf("Error: %d\n", error);
    if (error < 0)
    {
        exit (-1);
    }
    else
    {
        sleep(1);
/*        this->dataPointer = malloc(4096);
        Telegram_Register testtele(*(SocketAddressLocal*)(this->master.getLocalSocket()->getAddress()), std::string("TEST"));
        testtele.serialize(this->dataPointer);

        Telegram_Register testtele2;
        testtele2.deserialize(this->dataPointer);
*/        printf("Event System Master successful created\n");
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

void* EventSystemMaster::getDataPointer()
{
    return dataPointer;
}

void* EventSystemMaster::checkForMessage(void* arg)
{
	return nullptr;
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
}

void EventSystemMaster::sendToClient(std::string destination, void* data, int numOfBytes)
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
            this->master.send(data, numOfBytes, &address);
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
			this->master.send((void*)data, numOfBytes, &address);
		}
	}

}
