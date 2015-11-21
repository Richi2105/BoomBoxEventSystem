#ifndef SOCKETREADERMASTER_H
#define SOCKETREADERMASTER_H

#include <netinet/in.h>
#include <string>

#include "SocketAddressLocal.h"
#include "SocketAddressNetwork.h"
#include "SocketIOLocal.h"
#include "SocketIONetwork.h"


class Socket_Master
{
    public:
        Socket_Master(in_port_t port);
        virtual ~Socket_Master();

        int send(void* data, int numOfBytes, SocketAddressLocal* dest);
        int send(void* data, int numOfBytes, SocketAddressNetwork* dest);

        std::string getUniqueID();

        void broadcast();
    protected:
    private:
        SocketIO_Network* networkSocket;
        SocketIO_Local* localSocket;
        void* mapAddress;
};

#endif // SOCKETREADERMASTER_H
