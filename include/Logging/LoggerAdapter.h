/*
 * LoggerAdapter.h
 *
 *  Created on: Dec 12, 2015
 *      Author: richard
 */

#ifndef INCLUDE_LOGGING_LOGGERADAPTER_H_
#define INCLUDE_LOGGING_LOGGERADAPTER_H_

#include "../EventSystemParticipant.h"
#include "Log.h"
#include "../Telegram/TelegramObject.h"

namespace EventSystem
{

class LoggerAdapter {
public:
	virtual ~LoggerAdapter();

	static void initLoggerAdapter(EventSystemParticipant* espi);
	static void log(Log::level_t level, std::string message);


private:
	LoggerAdapter();
	static EventSystemParticipant* espi;
	static Log logObject;
	static Telegram_Object logTelegram;
};

} /* namespace EventSystem */

#endif /* INCLUDE_LOGGING_LOGGERADAPTER_H_ */
