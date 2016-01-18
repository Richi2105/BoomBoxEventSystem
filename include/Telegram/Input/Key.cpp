/*
 * Key.cpp
 *
 *  Created on: Dec 19, 2015
 *      Author: richard
 */

#include "Key.h"

std::string Key::KEY_KNOB_UP_STR = "KEY_KNOB_UP_VOLUME_UP";
std::string Key::KEY_KNOB_DOWN_STR = "KEY_KNOB_DOWN_VOLUME_DOWN";
std::string Key::KEY_MENU_STR = "KEY_MENU_BACK";
std::string Key::KEY_PLAY_STR = "KEY_PLAY";
std::string Key::KEY_NEXT_STR = "KEY_NEXT_RIGHT";
std::string Key::KEY_PREVIOUS_STR = "KEY_PREVIOUS_LEFT";
std::string Key::KEY_STOP_STR = "KEY_STOP_ESCAPE";
std::string Key::KEY_ENTER_STR = "KEY_ENTER_OK";

std::string Key::getKeyDescription(Key::key_type key)
{
	switch (key)
	{
	case Key::KEY_KNOB_UP: return Key::KEY_KNOB_UP_STR;
	case Key::KEY_KNOB_DOWN: return Key::KEY_KNOB_DOWN_STR;
	case Key::KEY_MENU: return Key::KEY_MENU_STR;
	case Key::KEY_ENTER: return Key::KEY_ENTER_STR;
	case Key::KEY_PLAY: return Key::KEY_PLAY_STR;
	case Key::KEY_STOP: return Key::KEY_STOP_STR;
	case Key::KEY_NEXT: return Key::KEY_NEXT_STR;
	case Key::KEY_PREVIOUS: return Key::KEY_PREVIOUS_STR;
	default: return "";
	}
}

Key::key_type Key::getFromDescription(std::string keyString)
{
	int i = 0;
	for ( ; i<KEYNUMBER; i+=1)
	{
		if (Key::getKeyDescription((Key::key_type) i).find(keyString) != std::string::npos)
		{
			break;
		}
	}
	return (Key::key_type) i;
}

Key::Key(std::string description, bool longpressed)
{
//	memset(this->keyDescription, 0, KEYDESCRIPTION_SIZE);
//    memcpy(this->keyDescription, description.c_str(), KEYDESCRIPTION_SIZE < description.size() ? KEYDESCRIPTION_SIZE : description.size());
	this->keyIdentifier = Key::getFromDescription(description);
	this->longpressed = longpressed;
}

Key::Key(Key::key_type, bool longpressed)
{
	this->keyIdentifier = keyIdentifier;
	this->longpressed = longpressed;
}

Key::~Key() {
	// TODO Auto-generated destructor stub
}

std::string Key::getKeyDescription()
{
//	return this->keyDescription;
	return Key::getKeyDescription(this->keyIdentifier);
}

Key::key_type Key::getKeyIdentifier()
{
	return this->keyIdentifier;
}

bool Key::isLongPressed()
{
	return longpressed;
}

void Key::setKeyDescription(std::string description)
{
//	memset(this->keyDescription, 0, KEYDESCRIPTION_SIZE);
//	memcpy(this->keyDescription, description.c_str(), KEYDESCRIPTION_SIZE < description.size() ? KEYDESCRIPTION_SIZE : description.size());
	this->keyIdentifier = Key::getFromDescription(description);
}

void Key::setKeyDescription(Key::key_type key)
{
	this->keyIdentifier = key;
}

void Key::setLongPressed(bool longpressed)
{
	this->longpressed = longpressed;
}

int Key::getSerializedSize()
{
	int16_t size = 0;
	size += sizeof(this->keyIdentifier);
	size += sizeof(this->longpressed);
	return size;
}

int Key::serialize(void* const data)
{
	MEMUNIT* data2 = (MEMUNIT*)data;
	packData(data2, this->keyIdentifier);
	packData(data2, this->longpressed);

	return this->getSerializedSize();
}

int Key::deserialize(void const * const data)
{
	const MEMUNIT* data2 = (MEMUNIT*)data;
	unpackData(data2, this->keyIdentifier);
	unpackData(data2, this->longpressed);

	return this->getSerializedSize();
}

