/*
 * SocketReader.h
 *
 *  Created on: Nov 20, 2015
 *      Author: richard
 */

#ifndef INCLUDE_SOCKETIO_H_
#define INCLUDE_SOCKETIO_H_

#include <string>
#include "SocketAddress.h"

class SocketIO {
public:
	virtual ~SocketIO() = 0;
	virtual int send(void* data, int numOfBytes) = 0;
	virtual int receive(void* data, int numOfBytes) = 0;
	virtual std::string getUniqueID() = 0;
	virtual SocketAddress* getAddress() = 0;
	virtual int getSocketFileDescriptor() = 0;
};

#endif /* INCLUDE_SOCKETIO_H_ */
