#ifndef EVENTSYSTEMMASTER_H
#define EVENTSYSTEMMASTER_H

#include <string>
#include <sys/un.h>

#include "EventSystemParticipant.h"
#include "SocketAddress.h"
#include "../include/Telegram/Telegram.h"

#include <vector>
#include <map>
#include <thread>

#include "SocketMaster.h"

typedef std::vector<SocketAddressLocal> LocalClientVector;
typedef std::vector<SocketAddressNetwork> NetworkClientVector;
typedef std::map<std::string, LocalClientVector> LocalClientMap;
typedef std::map<std::string, NetworkClientVector> NetworkClientMap;


typedef void* (threadfunction)(void* arg);

class EventSystemMaster : public EventSystemParticipant
{
    public:
        EventSystemMaster();
        virtual ~EventSystemMaster();

        std::string getIdentifier();
        std::string getUniqueIdentifier();
        virtual SocketIO* getSocket();
        SocketIO_Local* getLocalSocket();
        SocketIO_Network* getNetworkSocket();
        virtual SocketAddress* getAddress();
        void addClient(std::string id, SocketAddressLocal address);
        void addClient(std::string id, SocketAddressNetwork address);
        void sendToClient(std::string destination, Telegram* data);

        void* getDataPointer();

        void* checkForMessage(void* arg);
    protected:
    private:
        std::thread* threadCheckMessageLocal;
        std::thread* threadCheckMessageNetwork;
        std::string id;
        std::string uniqueID;
        Socket_Master master;
        void* dataPointer;
        LocalClientMap localClients;
        NetworkClientMap networkClients;
        std::vector<std::string> clientRoles;


};

#endif // EVENTSYSTEMMASTER_H
