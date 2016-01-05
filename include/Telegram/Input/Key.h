/*
 * Key.h
 *
 *  Created on: Dec 19, 2015
 *      Author: richard
 */

#ifndef INCLUDE_TELEGRAM_INPUT_KEY_H_
#define INCLUDE_TELEGRAM_INPUT_KEY_H_

#define GLUE(x, y) x##y
#define makeString(x, tags) std::string Key::##x##_STR = #x+"_"+#tags
#define KEYNUMBER 8

#include "../../Serializeable.h"
#include "../../constants.h"
#include <string>

/**
 * this class represents a input key which could have been pressed
 */
class Key : public Serializeable
{
public:
	/**
	 * a representation of possible input keys
	 */
	enum key_type {KEY_KNOB_UP, KEY_KNOB_DOWN, KEY_MENU, KEY_ENTER, KEY_PLAY, KEY_STOP, KEY_NEXT, KEY_PREVIOUS};

	/**
	 * constructs a key object
	 * @param description: depending on the description, a key value of type key_type is chosen. description must be uppercase only.
	 * if conversion fails, KEY_STOP is the key value
	 * @param longpressed: indicates if the key has been pressed for a long time, say 1 sec
	 */
	Key(std::string description, bool longpressed);

	/**
	 * constructs a key object
	 * @param key: a value of key_type
	 * @param longpressed: indicates if the key has been pressed for a long time, say 1 sec
	 */
	Key(Key::key_type key, bool longpressed);
	virtual ~Key();

	/**
	 * @return: the textual description of the pressed key, including the designated functions
	 */
	std::string getKeyDescription();

	/**
	 * @return the key value
	 */
	Key::key_type getKeyIdentifier();

	/**
	 * @return if the key has been pressed a long time
	 */
	bool isLongPressed();

	/**
	 * set key to a new value, without constructing a new object
	 * @param description: textual description of the key, uppercase letters
	 */
	void setKeyDescription(std::string description);

	/**
	 * set key to a new value, without constructing a new object
	 * @param key: the new value
	 */
	void setKeyDescription(Key::key_type key);

	/**
	 * set if the key has been pressed long
	 * @param longpressed: indicates if the key has been pressed for a long time, say 1 sec
	 */
	void setLongPressed(bool longpressed);

	virtual int getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

	static std::string KEY_KNOB_UP_STR;
	static std::string KEY_KNOB_DOWN_STR;
	static std::string KEY_MENU_STR;
	static std::string KEY_ENTER_STR;
	static std::string KEY_PLAY_STR;
	static std::string KEY_STOP_STR;
	static std::string KEY_NEXT_STR;
	static std::string KEY_PREVIOUS_STR;


	/**
	 * get the textual description of a key value
	 * @return: the description
	 */
	static std::string getKeyDescription(Key::key_type key);

	/**
	 * get a value according to a textual description, uppercase letters only
	 * @return: the key value
	 */
	static Key::key_type getFromDescription(std::string keyString);

private:
//	char keyDescription[KEYDESCRIPTION_SIZE];
	Key::key_type keyIdentifier;
	bool longpressed;
};

#endif /* INCLUDE_TELEGRAM_INPUT_KEY_H_ */
