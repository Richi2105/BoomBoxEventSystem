/*
 * TelegramObject.h
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#ifndef INCLUDE_TELEGRAM_TELEGRAMOBJECT_H_
#define INCLUDE_TELEGRAM_TELEGRAMOBJECT_H_

#include "Telegram.h"

namespace Telegram
{

class Telegram_Object: public Telegram {
public:
	Telegram_Object(std::string destination, Serializeable* obj);
	Telegram_Object();
	virtual ~Telegram_Object();

    virtual int16_t getSerializedSize();
	virtual int serialize(void* const data);
	//@Override
	virtual int deserialize(void const * const data, Serializeable* obj);

	Serializeable* getObject();

private:
	Serializeable* object;
};

} /* namespace Telegram */

#endif /* INCLUDE_TELEGRAM_TELEGRAMOBJECT_H_ */
