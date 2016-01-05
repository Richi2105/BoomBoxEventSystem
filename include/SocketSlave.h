#ifndef SOCKETREADERSLAVE_H
#define SOCKETREADERSLAVE_H

#include <string>
#include <netinet/in.h>
#include <sys/socket.h>

#include "SocketIO.h"
#include "SocketAddress.h"
#include "SocketAddressLocal.h"

/**
 * this class represents a socket to communicate locally or per network
 * It has a SocketIO variable which is either a SocketIOLocal or SocketIONetwork type
 */
class Socket_Slave
{
public:
	/**
	 * constructs a socket for communication locally, trys to find the master address in shared memory
	 */
	Socket_Slave();

	/**
	 * constructs a socket for communication via network
	 * @param networkDevice: a string identifier of the network device, like eth0 or wlan0. If argument is NULL, an address containing 192.168. is used
	 */
	Socket_Slave(in_port_t port, char* networkDevice);
	virtual ~Socket_Slave();

	/**
	 * set the address to the master when communicating via network.
	 * Before communication can succeed, an address is needed.
	 */
	void setAddress(struct sockaddr_in address, socklen_t len);

	/**
	 * returns the address of this socket.
	 * In local mode, SocketAddressLocal type is returned.
	 * in network mode, the previous set SocketAddressNetwork type is returned.
	 */
	SocketAddress* getAddress();

	/**
	 * returns the actual socket for send() and receive() operation.
	 */
	SocketIO* getSocket();

	/**
	 * returns whether this socket was initialized local or network mode
	 */
	bool isLocal();

	/**
	 * sends data
	 * @param data: the data (duh)
	 * @param numOfBytes: the number of data to be sent
	 */
	int send(void* data, int numOfBytes);

	/**
	 * receives data
	 * @param data: the data written to
	 * @param numOfBytes: the maximum number of data to be received
	 * @return the actual bytes received
	 */
	int receive(void* data, int numOfBytes);

	/**
	 * @return: a unique string consisting of the Process Identification Number of this process.
	 */
	std::string getUniqueID();

protected:
private:
	bool local;
	SocketAddress* masterAddress;

	SocketIO* socket;

	void getAddressFromSharedMemory(SocketAddressLocal** address);
};

#endif // SOCKETREADERSLAVE_H
