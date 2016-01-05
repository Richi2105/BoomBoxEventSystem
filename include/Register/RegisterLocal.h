/*
 * RegisterLocal.h
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#ifndef INCLUDE_REGISTER_REGISTERLOCAL_H_
#define INCLUDE_REGISTER_REGISTERLOCAL_H_

#include <string>
#include "../constants.h"
#include "RegisterInterface.h"
#include "../SocketAddressLocal.h"

namespace EventSystem {

class Register_Local : public Register_Interface
{
public:
	Register_Local();
	Register_Local(SocketAddressLocal* address, std::string clientName);
	virtual ~Register_Local();

    virtual SocketAddressLocal* getClientAddress();
    virtual char* getClientID();

    virtual int getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

private:
	SocketAddressLocal* clientAddress;
	char clientID[ID_SIZE];
};

} /* namespace EventSystem */

#endif /* INCLUDE_REGISTER_REGISTERLOCAL_H_ */
