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

class SocketIO_Network : public SocketIO
{
public:
	SocketIO_Network(in_port_t port);
	virtual ~SocketIO_Network();

	virtual int send(void* data, int numOfBytes);
	virtual int receive(void* data, int numOfBytes);

	virtual SocketAddress* getAddress();

	virtual int getSocketFileDescriptor();

	virtual std::string getUniqueID();

private:
	SocketAddressNetwork myAddress;
	int socketFileDescriptor;
	std::string uniqueID;
};

#endif /* INCLUDE_SOCKETIONETWORK_H_ */
