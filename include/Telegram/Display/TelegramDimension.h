/*
 * TelegramDimension.h
 *
 *  Created on: Dec 21, 2015
 *      Author: richard
 */

#ifndef INCLUDE_TELEGRAM_DISPLAY_TELEGRAMDIMENSION_H_
#define INCLUDE_TELEGRAM_DISPLAY_TELEGRAMDIMENSION_H_

#include "../Telegram.h"

class Telegram_Dimension : public Telegram
{
public:
	Telegram_Dimension(int x, int y);
	virtual ~Telegram_Dimension();

	virtual int16_t getSerializedSize() = 0;
	virtual int serialize(void* const data) = 0;
	virtual int deserialize(void const * const data) = 0;

private:
	int xResolution;
	int yResolution;
};

#endif /* INCLUDE_TELEGRAM_DISPLAY_TELEGRAMDIMENSION_H_ */
