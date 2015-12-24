#include "../include/Telegram/Telegram_Display.h"
#include <OS_DEF.h>

Telegram_Display::Telegram_Display(DisplayPosition pos, std::string dispstr) : Telegram("DISPLAY")
{
    memcpy(this->str, dispstr.c_str(), dispstr.size());
    this->pos = pos;
    this->telegramSize += STRINGSIZE + sizeof(DisplayPosition);

}

Telegram_Display::Telegram_Display() : Telegram("DISPLAY"), pos()
{

}

Telegram_Display::~Telegram_Display()
{

}

DisplayPosition Telegram_Display::getPosition()
{
    return this->pos;
}

char* Telegram_Display::getDisplayString()
{
    return this->str;
}
int16_t Telegram_Display::getSerializedSize()
{
	printf("Telegram_Display::getSerializedSize()\n");
	int16_t size = 0;
	size += Telegram::getSerializedSize();
	size += this->pos.getSerializedSize();
	size += sizeof(this->str[0])*STRINGSIZE;

	return size;
}
int Telegram_Display::serialize(void* const data)
{
	printf("Telegram_Display::serialize(void* const data)\n");
	//void* data = malloc(this->getSerializedSize());
	MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::serialize(data2);
	data2 += this->pos.serialize(data2);
	packNData(data2, this->str, STRINGSIZE);

	return this->getSerializedSize();
}
int Telegram_Display::deserialize(void const * const data)
{
	printf("Telegram_Display::deserialize(void const * const data)\n");
	const MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::deserialize(data2);
	data2 += this->pos.deserialize(data2);
	unpackNData(data2, this->str, STRINGSIZE);

	return this->getSerializedSize();
}

