/*
 * RegisterInterface.h
 *
 *  Created on: Dec 23, 2015
 *      Author: richard
 */

#ifndef INCLUDE_REGISTER_REGISTERINTERFACE_H_
#define INCLUDE_REGISTER_REGISTERINTERFACE_H_

#include "../Serializeable.h"
#include "../SocketAddress.h"

namespace EventSystem {

class Register_Interface : public Serializeable {
public:
	virtual ~Register_Interface();

    virtual SocketAddress* getClientAddress() = 0;
    virtual char* getClientID() = 0;

    virtual int16_t getSerializedSize() = 0;
	virtual int serialize(void* const data) = 0;
	virtual int deserialize(void const * const data) = 0;
};

} /* namespace EventSystem */

#endif /* INCLUDE_REGISTER_REGISTERINTERFACE_H_ */
