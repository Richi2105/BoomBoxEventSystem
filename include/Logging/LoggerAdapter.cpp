/*
 * LoggerAdapter.cpp
 *
 *  Created on: Dec 12, 2015
 *      Author: richard
 */

#include "LoggerAdapter.h"
#include "../Telegram/Telegram_Log.h"
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

void LoggerAdapter::initLoggerAdapter(EventSystemParticipant* espi)
{
	LoggerAdapter::espi = espi;
}

void LoggerAdapter::log(LoggerAdapter::level_t level, std::string message)
{
	if (LoggerAdapter::espi == nullptr)
	{
		if (level == LoggerAdapter::SEVERE)
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
		Log* log = new Log(LoggerAdapter::espi, message, level);
		Telegram_Object* objTelegram = new Telegram_Object("LOGGER", log);
		objTelegram->setType(Telegram::LOG);
		espi->log(objTelegram);
	}
}

} /* namespace EventSystem */


