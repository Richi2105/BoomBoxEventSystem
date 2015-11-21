#ifndef SOCKETREADER_H
#define SOCKETREADER_H

#include <string>
#include <sys/socket.h>
#include <sys/un.h>

#include "SocketAddress.h"
#include "SocketAddressLocal.h"
#include "SocketIO.h"

#define PORT 666

class SocketIO_Local : public SocketIO
{
    public:
        SocketIO_Local();
        virtual ~SocketIO_Local();

        virtual int send(void* data, int numOfBytes);
        int receive(void* data, int numOfBytes);

        SocketAddress* getAddress();

        int getSocketFileDescriptor();

        std::string getUniqueID();

    protected:
        SocketAddressLocal myAddress;
        int socketFileDescriptor;
        std::string uniqueID;
    private:
};

#endif // SOCKETREADER_H