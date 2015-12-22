/*
 * Key.h
 *
 *  Created on: Dec 19, 2015
 *      Author: richard
 */

#ifndef INCLUDE_TELEGRAM_INPUT_KEY_H_
#define INCLUDE_TELEGRAM_INPUT_KEY_H_

#define KEYDESCRIPTION_SIZE 20
#define makeString(x) std::string Key::x = #x

#include "../../Serializeable.h"
#include <string>

class Key : public Serializeable
{
public:
	Key(std::string description, bool longpressed);
	virtual ~Key();

	char* getKeyDescription();
	bool isLongPressed();

	void setKeyDescription(std::string description);

	virtual int16_t getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

	static std::string KEY_KNOB_UP;
	static std::string KEY_KNOB_DOWN;
	static std::string KEY_MENU;
	static std::string KEY_ENTER;
	static std::string KEY_PLAY;
	static std::string KEY_STOP;
	static std::string KEY_NEXT;
	static std::string KEY_PREVIOUS;

private:
	char keyDescription[KEYDESCRIPTION_SIZE];
	bool longpressed;
};

#endif /* INCLUDE_TELEGRAM_INPUT_KEY_H_ */
