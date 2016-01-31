/*
 * StateMachine.h
 *
 *  Created on: Jan 21, 2016
 *      Author: richard
 */

#ifndef INCLUDE_STATEMACHINE_STATEMACHINE_H_
#define INCLUDE_STATEMACHINE_STATEMACHINE_H_

#include "../constants.h"
#include <OS_DEF.h>

#include <string>
#include <vector>
#include <pthread.h>

namespace EventSystem {

class StateMachine {
public:
	static const uint8 EVENTSYSTEM_STATE_IDLE = 0;
	static const uint8 EVENTSYSTEM_STATE_RECEIVING = 1;
	static const uint8 EVENTSYSTEM_STATE_STOPPED = 2;
	static const uint8 EVENTSYSTEM_STATE_DONE = 3;
	static const uint8 EVENTSYSTEM_STATE_CUSTOM = 4;

	/**
	 * creates a new StateMachine
	 */
	StateMachine();
	virtual ~StateMachine();

	/**
	 * sets the state, either a given state or a prior specified custom state
	 * thread safe
	 * @see specifyCustomState()
	 */
	void setState(uint8 state);

	/**
	 * @return the state specified via setState()
	 * thread safe
	 */
	uint8 getState();

	/**
	 * returns a description of a current default state, or the description of the custom state
	 * thread safe
	 */
	const char* getStateDescription();

	/**
	 * specify a custom state. returns the value of the new state, so you can set it via setState()
	 * @param customState a description of a new state
	 * @return the number of the new state
	 */
	uint8 specifyCustomState(std::string customState);

	/**
	 * @return a string description of a default state (<=EVENTSYSTEM_STATE_CUSTOM)
	 */
	static const char* getDefaultStateDescription(uint8 stateNr);

private:
	uint8 currentState;
	std::vector<std::string> customStates;
	pthread_mutex_t memberMutex;
};

} /* namespace EventSystem */

#endif /* INCLUDE_STATEMACHINE_STATEMACHINE_H_ */
