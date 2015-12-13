/*
 * LoggerAdapter.cpp
 *
 *  Created on: Dec 12, 2015
 *      Author: richard
 */

#include "LoggerAdapter.h"
#include "../Telegram/Telegram_Log.h"
#include "../Telegram/Telegram.h"
#include <stdio.h>

LoggerAdapter::LoggerAdapter() {
	// TODO Auto-generated constructor stub

}

LoggerAdapter::~LoggerAdapter() {
	// TODO Auto-generated destructor stub
}

EventSystemParticipantImpl* LoggerAdapter::espi = nullptr;

void LoggerAdapter::initLoggerAdapter(EventSystemParticipantImpl* espi)
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
		Telegram* telegram = new Telegram_Log(LoggerAdapter::espi, message, level);
		espi->send(telegram);
	}
}


