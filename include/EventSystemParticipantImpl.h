#ifndef EVENTSYSTEMPARTICIPANTIMPL_H
#define EVENTSYSTEMPARTICIPANTIMPL_H

#include "Telegram/Telegram.h"
#include <string>
#include <pthread.h>

#include "EventSystemParticipant.h"
#include "SocketSlave.h"


class EventSystemParticipantImpl : public EventSystemParticipant
{
    public:
        EventSystemParticipantImpl(std::string);
        virtual ~EventSystemParticipantImpl();
        std::string getIdentifier();
        std::string getUniqueIdentifier();
        SocketAddress* getAddress();
        virtual SocketIO* getSocket();
        void* getMessageMemory();

        void send(Telegram* telegram);
        int receive(Telegram* telegram, bool nonblocking);
        void setMessageReceived(bool newMessage);
    protected:
    private:
        std::string id;
        Socket_Slave socket;
        void* messageMemory;    //Mutex!
        pthread_t connectThreadID;
        bool newMessage;        //Mutex!
};

#endif // EVENTSYSTEMPARTICIPANTIMPL_H
