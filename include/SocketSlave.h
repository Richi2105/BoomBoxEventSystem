#ifndef SOCKETREADERSLAVE_H
#define SOCKETREADERSLAVE_H

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>

#include "SocketIO.h"
#include "SocketAddress.h"
#include "SocketAddressLocal.h"


class Socket_Slave
{
    public:
        Socket_Slave();
        Socket_Slave(in_port_t port);
        virtual ~Socket_Slave();
        void setAddress(struct sockaddr_in address, socklen_t len);
        SocketAddress* getAddress();
        SocketIO* getSocket();

        int send(void* data, int numOfBytes);

        std::string getUniqueID();

        void connect(std::string id);
    protected:
    private:
        bool local;
        SocketAddress* masterAddress;

        SocketIO* socket;

        void getAddressFromSharedMemory(SocketAddressLocal** address);
};

#endif // SOCKETREADERSLAVE_H
