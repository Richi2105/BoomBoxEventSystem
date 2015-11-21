#ifndef EVENTRECEIVERNODE_H
#define EVENTRECEIVERNODE_H

#include "SocketMaster.h"


class EventReceiverNode
{
    public:
        EventReceiverNode();
        virtual ~EventReceiverNode();
    protected:
    private:
        Socket_Master master;
};

#endif // EVENTRECEIVERNODE_H
