/*
 * TelegramObject.cpp
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#include "TelegramObject.h"

namespace Telegram
{

Telegram_Object::Telegram_Object(std::string destination, Serializeable* obj) : Telegram(destination)
{
	this->object = obj;
}

Telegram_Object::Telegram_Object() : Telegram("nothing")
{
	this->object = nullptr;
}

Telegram_Object::~Telegram_Object() {
	// TODO Auto-generated destructor stub
}

Serializeable* Telegram_Object::getObject()
{
	return this->object;
}

int16_t Telegram_Object::getSerializedSize()
{
	printf("Telegram_Object::getSerializedSize()\n");
	int16_t size = 0;
	size += Telegram::getSerializedSize();
	size += this->object->getSerializedSize();

	return size;
}
int Telegram_Object::serialize(void* const data)
{
	printf("Telegram_Object::serialize(void* const data)\n");
	//void* data = malloc(this->getSerializedSize());
	MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::serialize(data2);
	data2 += this->object->serialize(data2);

	return this->getSerializedSize();
}
int Telegram_Object::deserialize(void const * const data, Serializeable* obj)
{
	printf("Telegram_Object::deserialize(void const * const data)\n");
	const MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::deserialize(data2);
	data2 += obj->deserialize(data2);
	this->object = obj;

	return this->getSerializedSize();
}

} /* namespace Telegram */

