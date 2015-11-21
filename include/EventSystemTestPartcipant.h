#ifndef EVENTSYSTEMTESTPARTCIPANT_H
#define EVENTSYSTEMTESTPARTCIPANT_H

#include "EventSystemParticipant.h"
#include <string>

#include "SocketSlave.h"


class EventSystemTestPartcipant : public EventSystemParticipant
{
    public:
        EventSystemTestPartcipant();
        virtual ~EventSystemTestPartcipant();

        std::string getIdentifier()
        {
            return this->id;
        }
        std::string getUniqueIdentifier()
        {
            return (this->slave.getUniqueID() + "__" + this->id);
        }
        sockaddr_un* getAddress()
        {
            return this->slave.getAddress();
        }
        virtual SocketIO_Local* getSocket()
        {
            return &(this->slave);
        }

        void send(std::string text);

    protected:
    private:
        std::string id;
        std::string uniqueID;
        Socket_Slave slave;
};

#endif // EVENTSYSTEMTESTPARTCIPANT_H
