/*
 * TelegramObject.cpp
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#include "TelegramObject.h"

namespace EventSystem
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

void Telegram_Object::setObject(Serializeable* obj)
{
	this->object = obj;
}

int Telegram_Object::getSerializedSize()
{
	#ifdef DEBUG_OUT
	printf("Telegram_Object::getSerializedSize()\n");
	#endif //DEBUG_OUT
	int16_t size = 0;
	size += Telegram::getSerializedSize();
	size += this->object->getSerializedSize();

	return size;
}
int Telegram_Object::serialize(void* const data)
{
	#ifdef DEBUG_OUT
	printf("Telegram_Object::serialize(void* const data)\n");
	#endif //DEBUG_OUT
	//void* data = malloc(this->getSerializedSize());
	MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::serialize(data2);
	data2 += this->object->serialize(data2);

	return this->getSerializedSize();
}
int Telegram_Object::deserialize(void const * const data, Serializeable* obj)
{
	#ifdef DEBUG_OUT
	printf("Telegram_Object::deserialize(void const * const data)\n");
	#endif //DEBUG_OUT
	const MEMUNIT* data2 = (MEMUNIT*)data;
	data2 += Telegram::deserialize(data2);
	data2 += obj->deserialize(data2);
	this->object = obj;

	return this->getSerializedSize();
}

} /* namespace EventSystem */

