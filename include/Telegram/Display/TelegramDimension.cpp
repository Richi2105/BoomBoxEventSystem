/*
 * TelegramDimension.cpp
 *
 *  Created on: Dec 21, 2015
 *      Author: richard
 */

#include "TelegramDimension.h"

Telegram_Dimension::Telegram_Dimension(int x, int y) : Telegram("DISPLAY_CLIENT")
{
	this->xResolution = x;
	this->yResolution = y;

}

Telegram_Dimension::~Telegram_Dimension() {
	// TODO Auto-generated destructor stub
}

int16_t Telegram_Dimension::getSerializedSize()
{
	int16_t size = 0;
	size += Telegram::getSerializedSize();
	size += sizeof(this->xResolution);
	size += sizeof(this->yResolution);

	return size;
}
int Telegram_Dimension::serialize(void* const data)
{
	MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::serialize(data2);
	packData(data2, this->xResolution);
	packData(data2, this->yResolution);

	return Telegram::getSerializedSize();
}
int Telegram_Dimension::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::deserialize(data2);
	unpackData(data2, this->xResolution);
	unpackData(data2, this->yResolution);

	return Telegram::getSerializedSize();
}

