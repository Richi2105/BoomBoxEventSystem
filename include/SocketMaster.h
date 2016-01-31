#ifndef SOCKETREADERMASTER_H
#define SOCKETREADERMASTER_H

#include <netinet/in.h>
#include <string>

#include "SocketAddressLocal.h"
#include "SocketAddressNetwork.h"
#include "SocketAddress.h"
#include "SocketIOLocal.h"
#include "SocketIONetwork.h"

/**
 * this class contains two sockets
 */
class Socket_Master
{
    public:
		/**
		 * init the local socket
		 * init the network socket to have the address of a network device containing 192.168.
		 */
        Socket_Master(in_port_t port);

        /**
         * init the local socket
         * init the network socket to have the address of the specified device
         */
        Socket_Master(char* networkDevice, in_port_t port);
        virtual ~Socket_Master();

        /**
         * send stuff to the specified address
         * in this function it is determined if it is local send or network send
         */
        int send(void* data, int numOfBytes, SocketAddress* dest);

        /**
         * send stuff to the specified address locally
         */
        int send(void* data, int numOfBytes, SocketAddressLocal* dest);

        /**
         * send stuff to the specified address via network
         */
        int send(void* data, int numOfBytes, SocketAddressNetwork* dest);

        SocketIO_Network* getNetworkSocket();
        SocketIO_Local* getLocalSocket();

        std::string getUniqueID();

        void broadcast();
    protected:
    private:
        SocketIO_Network* networkSocket;
        SocketIO_Local* localSocket;
        void* mapAddress;
        int shmFileDescriptor;
};

#endif // SOCKETREADERMASTER_H
