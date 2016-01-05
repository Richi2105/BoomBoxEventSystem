/*
 * RegisterNetwork.h
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#ifndef INCLUDE_REGISTER_REGISTERNETWORK_H_
#define INCLUDE_REGISTER_REGISTERNETWORK_H_

#include <string>
#include "../constants.h"
#include "RegisterInterface.h"
#include "../SocketAddressNetwork.h"

namespace EventSystem {

class Register_Network : public Register_Interface
{
public:
	Register_Network();
	Register_Network(SocketAddressNetwork* address, std::string clientName);
	virtual ~Register_Network();

    virtual SocketAddressNetwork* getClientAddress();
    virtual char* getClientID();

    virtual int getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

private:
	SocketAddressNetwork* clientAddress;
	char clientID[ID_SIZE];
};

} /* namespace EventSystem */

#endif /* INCLUDE_REGISTER_REGISTERNETWORK_H_ */
