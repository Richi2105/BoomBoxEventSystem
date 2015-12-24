#ifndef SOCKETREADER_H
#define SOCKETREADER_H

#include <string>
#include <sys/socket.h>
#include <sys/un.h>

#include "SocketAddress.h"
#include "SocketAddressLocal.h"
#include "SocketIO.h"

/**
 * this class contains the file descriptor to the local socket and it's own address
 */
class SocketIO_Local : public SocketIO
{
public:
	/**
	 * constructs a socket and the belonging address
	 */
	SocketIO_Local();
	virtual ~SocketIO_Local();

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
	 * @return the address of the socket
	 */
	virtual SocketAddress* getAddress();

	/**
	 * @return the file descriptor of the socket
	 */
	virtual int getSocketFileDescriptor();

	/**
	 * @return the unique id
	 * @see SocketSlave::getUniqueID
	 */
	virtual std::string getUniqueID();

protected:
	SocketAddressLocal myAddress;
	int socketFileDescriptor;
	std::string uniqueID;
private:
};

#endif // SOCKETREADER_H
