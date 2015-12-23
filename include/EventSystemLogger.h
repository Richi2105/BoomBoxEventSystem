#ifndef EVENTSYSTEMLOGGER_H
#define EVENTSYSTEMLOGGER_H

#include <string>

#include "EventSystemParticipant.h"
#include "SocketSlave.h"
#include "SocketAddress.h"


class EventSystemLogger : public EventSystemParticipant
{
    public:
        EventSystemLogger();
        virtual ~EventSystemLogger();

        std::string getIdentifier();

        std::string getUniqueIdentifier();

        SocketAddress* getAddress();

        SocketIO* getSocket();

        void log();

    protected:
    private:
        std::string id;
        Socket_Slave slave;
};

#endif // EVENTSYSTEMLOGGER_H
