/*
 * TelegramObject.h
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#ifndef INCLUDE_TELEGRAM_TELEGRAMOBJECT_H_
#define INCLUDE_TELEGRAM_TELEGRAMOBJECT_H_

#include "Telegram.h"

namespace EventSystem
{

class Telegram_Object: public Telegram {
public:
	Telegram_Object(std::string destination, Serializeable* obj);
	Telegram_Object();
	virtual ~Telegram_Object();

    virtual int getSerializedSize();
	virtual int serialize(void* const data);
	//@Override
	virtual int deserialize(void const * const data, Serializeable* obj);

	Serializeable* getObject();
	void setObject(Serializeable* obj);

private:
	Serializeable* object;
};

} /* namespace EventSystem */

#endif /* INCLUDE_TELEGRAM_TELEGRAMOBJECT_H_ */
