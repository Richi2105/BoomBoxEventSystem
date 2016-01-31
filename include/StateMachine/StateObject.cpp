/*
 * StateObject.cpp
 *
 *  Created on: Jan 21, 2016
 *      Author: richard
 */

#include "StateObject.h"
#include "StateMachine.h"

namespace EventSystem {

StateObject::StateObject() {
	memset(this->customState, 0, CUSTOMSTATE);
	this->state = StateMachine::EVENTSYSTEM_STATE_IDLE;
}

StateObject::StateObject(uint8 state, std::string customState)
{
	memset(this->customState, 0, CUSTOMSTATE);
	this->state = 0;
	this->setState(state, customState);
}

StateObject::~StateObject() {
	// TODO Auto-generated destructor stub
}

int8 StateObject::getState()
{
	return this->state;
}
char* StateObject::getCustomState()
{
	return this->customState;
}
void StateObject::setState(uint8 state, std::string customState)
{
	if (state > StateMachine::EVENTSYSTEM_STATE_CUSTOM)
	{
		memcpy(this->customState, customState.c_str(), CUSTOMSTATE < customState.size() ? CUSTOMSTATE : customState.size());
	}

	this->state = state;
}

int StateObject::getSerializedSize()
{
	int size = 0;
	size += sizeof(this->state);
	if (this->state > StateMachine::EVENTSYSTEM_STATE_CUSTOM)
		size += sizeof(this->customState[0]) * CUSTOMSTATE;
	return size;
}
int StateObject::serialize(void* const data)
{
	MEMUNIT* data2 = (MEMUNIT*)data;
	packData(data2, this->state);
	if (this->state > StateMachine::EVENTSYSTEM_STATE_CUSTOM)
		packNData(data2, this->customState, CUSTOMSTATE);
	return this->getSerializedSize();
}
int StateObject::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	unpackData(data2, this->state);
	if (this->state > StateMachine::EVENTSYSTEM_STATE_CUSTOM)
		unpackNData(data2, this->customState, CUSTOMSTATE);
	return this->getSerializedSize();
}

} /* namespace EventSystem */
