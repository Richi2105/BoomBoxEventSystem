/*
 * StateObject.h
 *
 *  Created on: Jan 21, 2016
 *      Author: richard
 */

#ifndef INCLUDE_STATEMACHINE_STATEOBJECT_H_
#define INCLUDE_STATEMACHINE_STATEOBJECT_H_

#include "../constants.h"
#include <OS_DEF.h>
#include "../Serializeable.h"

#include <string>

namespace EventSystem {

class StateObject: public Serializeable {
public:
	StateObject();
	StateObject(uint8 state, std::string customState = "");
	virtual ~StateObject();

	int8 getState();
	char* getCustomState();
	void setState(uint8 state, std::string customState = "");

	virtual int getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

private:
	int8 state;
	char customState[CUSTOMSTATE];
};

} /* namespace EventSystem */

#endif /* INCLUDE_STATEMACHINE_STATEOBJECT_H_ */
