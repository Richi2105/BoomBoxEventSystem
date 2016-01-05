/*
 * LoggerAdapter.cpp
 *
 *  Created on: Dec 12, 2015
 *      Author: richard
 */

#include "LoggerAdapter.h"
#include "Log.h"
#include "../Telegram/TelegramObject.h"
#include <stdio.h>

namespace EventSystem
{

LoggerAdapter::LoggerAdapter() {
	// TODO Auto-generated constructor stub

}

LoggerAdapter::~LoggerAdapter() {
	// TODO Auto-generated destructor stub
}

EventSystemParticipant* LoggerAdapter::espi = nullptr;
Log LoggerAdapter::logObject;
Telegram_Object LoggerAdapter::logTelegram(Telegram::ID_LOGGER, &LoggerAdapter::logObject);

void LoggerAdapter::initLoggerAdapter(EventSystemParticipant* espi)
{
	LoggerAdapter::espi = espi;
	LoggerAdapter::logObject.setSource(espi);
	LoggerAdapter::logTelegram.setType(Telegram::LOG);
}

void LoggerAdapter::log(Log::level_t level, std::string message)
{
	if (LoggerAdapter::espi == nullptr)
	{
		if (level == Log::SEVERE)
		{
			fprintf(stderr, "Severe:\nMessage: %s\n", message.c_str());
		}
		else
		{
			fprintf(stdout, "Level: %d\nMessage: %s\n", level, message.c_str());
		}
	}
	else
	{
		LoggerAdapter::logObject.setLog(message, level);
		espi->log(&LoggerAdapter::logTelegram);
	}
}

} /* namespace EventSystem */


