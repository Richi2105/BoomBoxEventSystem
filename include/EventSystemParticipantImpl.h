#ifndef EVENTSYSTEMPARTICIPANTIMPL_H
#define EVENTSYSTEMPARTICIPANTIMPL_H

#include "Telegram/Telegram.h"
#include <string>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "EventSystemParticipant.h"
#include "SocketSlave.h"


class EventSystemParticipantImpl : public EventSystemParticipant
{
    public:
        EventSystemParticipantImpl(std::string);
        EventSystemParticipantImpl(std::string, in_port_t port);
        virtual ~EventSystemParticipantImpl();
        int connectToMaster();
        int connectToMaster(sockaddr_in address);
        std::string getIdentifier();
        std::string getUniqueIdentifier();
        SocketAddress* getAddress();
        void setAddress(sockaddr_in address, socklen_t len);
        virtual SocketIO* getSocket();
        void* getMessageMemory();

        void send(Telegram* telegram);
        int receive(void* data, bool nonblocking);
        void setMessageReceived(bool newMessage);
    protected:
    private:
        std::string id;
        Socket_Slave socket;
        void* messageMemory;    //Mutex!
        void* sendMemory;
        pthread_t connectThreadID;
        bool newMessage;        //Mutex!
};

#endif // EVENTSYSTEMPARTICIPANTIMPL_H
