/*
 * Key.cpp
 *
 *  Created on: Dec 19, 2015
 *      Author: richard
 */

#include "Key.h"

makeString(KEY_KNOB_UP);
makeString(KEY_KNOB_DOWN);
makeString(KEY_MENU);
makeString(KEY_PLAY);
makeString(KEY_NEXT);
makeString(KEY_PREVIOUS);
makeString(KEY_STOP);
makeString(KEY_ENTER);

Key::Key(std::string description, bool longpressed)
{
	memcpy(this->keyDescription, description.c_str(), description.size());
	this->longpressed = longpressed;
}

Key::~Key() {
	// TODO Auto-generated destructor stub
}

char* Key::getKeyDescription()
{
	return this->keyDescription;
}

bool Key::isLongPressed()
{
	return longpressed;
}

int16_t Key::getSerializedSize()
{
	int16_t size = 0;
	size += sizeof(char)*KEYDESCRIPTION_SIZE;
	size += sizeof(longpressed);
	return size;
}

int Key::serialize(void* const data)
{
	MEMUNIT* data2 = (MEMUNIT*)data;
	packNData(data2, this->keyDescription, KEYDESCRIPTION_SIZE);
	packData(data2, this->longpressed);

	return this->getSerializedSize();
}

int Key::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	unpackNData(data2, this->keyDescription, KEYDESCRIPTION_SIZE);
	unpackData(data2, this->longpressed);

	return this->getSerializedSize();
}

