/*
 * LoggerAdapter.h
 *
 *  Created on: Dec 12, 2015
 *      Author: richard
 */

#ifndef INCLUDE_LOGGING_LOGGERADAPTER_H_
#define INCLUDE_LOGGING_LOGGERADAPTER_H_

#include "../EventSystemParticipant.h"

namespace EventSystem
{

class LoggerAdapter {
public:
	virtual ~LoggerAdapter();

	enum level_t {INFO, WARNING, SEVERE};

	static void initLoggerAdapter(EventSystemParticipant* espi);
	static void log(LoggerAdapter::level_t level, std::string message);


private:
	LoggerAdapter();
	static EventSystemParticipant* espi;
};

} /* namespace EventSystem */

#endif /* INCLUDE_LOGGING_LOGGERADAPTER_H_ */
