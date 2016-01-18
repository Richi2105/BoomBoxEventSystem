/*
 * SocketReaderNetwork.h
 *
 *  Created on: Nov 20, 2015
 *      Author: richard
 */

#ifndef INCLUDE_SOCKETIONETWORK_H_
#define INCLUDE_SOCKETIONETWORK_H_

#include <string>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/in.h>

#include "SocketAddressNetwork.h"
#include "SocketIO.h"

/**
 * this class contains the file descriptor to the network socket and it's own address
 */
class SocketIO_Network : public SocketIO
{
public:
	/**
	 * constructs a socket, the belonging address is taken
	 * a: by IP address (address is 192.168.xxx.xxx)
	 * b: by device name, if paramter devive != nullptr
	 */
	SocketIO_Network(in_port_t port, char* device);

	/**
	 * directly assign an address to a new socket
	 */
	SocketIO_Network(sockaddr_in* address);
	virtual ~SocketIO_Network();

	/**
	 * send stuff
	 */
	virtual int send(void* data, int numOfBytes);

	/**
	 * receive stuff, blocks when no data available
	 * @param data: the data the received stuff is written to
	 * @param numOfBytes: the maximum bytes to be received
	 */
	virtual int receive(void* data, int numOfBytes);

	/**
	 * @return: the network address of this socket
	 */
	virtual SocketAddress* getAddress();

	/**
	 * @return: the socket file descriptor
	 */
	virtual int getSocketFileDescriptor();

	/**
	 * @return the unique id
	 * @see SocketSlave::getUniqueID
	 */
	virtual std::string getUniqueID();

private:
	SocketAddressNetwork* myAddress;
	int socketFileDescriptor;
	std::string uniqueID;
};

#endif /* INCLUDE_SOCKETIONETWORK_H_ */
