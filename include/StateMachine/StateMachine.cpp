/*
 * StateMachine.cpp
 *
 *  Created on: Jan 21, 2016
 *      Author: richard
 */

#include "StateMachine.h"

namespace EventSystem {

const char* statedescriptions[] = {
		"idle",
		"receiving",
		"stopped",
		"done",
		"custom" //Should not be visible
};

StateMachine::StateMachine() {
	this->currentState = StateMachine::EVENTSYSTEM_STATE_IDLE;
	pthread_mutex_init(&this->memberMutex, NULL);
}

StateMachine::~StateMachine() {
	pthread_mutex_destroy(&this->memberMutex);
}

void StateMachine::setState(uint8 state)
{
pthread_mutex_lock(&this->memberMutex);
	this->currentState = state;
pthread_mutex_unlock(&this->memberMutex);
}

uint8 StateMachine::getState()
{
	uint8 retVal;
pthread_mutex_lock(&this->memberMutex);
	retVal = this->currentState;
pthread_mutex_unlock(&this->memberMutex);
	return retVal;
}

const char* StateMachine::getStateDescription()
{
	const char* retVal;
pthread_mutex_lock(&this->memberMutex);
	if (this->currentState > StateMachine::EVENTSYSTEM_STATE_CUSTOM)
	{
		unsigned int index = this->currentState - StateMachine::EVENTSYSTEM_STATE_CUSTOM - 1;
		if (index < this->customStates.size())
			retVal = (this->customStates.at(index)).c_str();
		else
			retVal = statedescriptions[StateMachine::EVENTSYSTEM_STATE_CUSTOM];
	}
	else
	{
		retVal = statedescriptions[this->currentState];
	}
pthread_mutex_unlock(&this->memberMutex);
	return retVal;
}

uint8 StateMachine::specifyCustomState(std::string customState)
{
	this->customStates.push_back(customState);
	return this->customStates.size() + StateMachine::EVENTSYSTEM_STATE_CUSTOM;
}

const char* StateMachine::getDefaultStateDescription(uint8 stateNr)
{
	if (stateNr <= EVENTSYSTEM_STATE_CUSTOM)
		return statedescriptions[stateNr];
	else
		return statedescriptions[StateMachine::EVENTSYSTEM_STATE_CUSTOM];
}

} /* namespace EventSystem */
