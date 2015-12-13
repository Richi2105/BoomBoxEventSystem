/*
 * LoggerAdapter.h
 *
 *  Created on: Dec 12, 2015
 *      Author: richard
 */

#ifndef INCLUDE_LOGGING_LOGGERADAPTER_H_
#define INCLUDE_LOGGING_LOGGERADAPTER_H_

#include "../EventSystemParticipantImpl.h"

class LoggerAdapter {
public:
	virtual ~LoggerAdapter();

	static enum level_t {INFO, WARNING, SEVERE} Level;

	static void initLoggerAdapter(EventSystemParticipantImpl* espi);
	static void log(LoggerAdapter::level_t level, std::string message);


private:
	LoggerAdapter();
	static EventSystemParticipantImpl* espi;
};

#endif /* INCLUDE_LOGGING_LOGGERADAPTER_H_ */
