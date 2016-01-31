/*
 * TelegramObject.h
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#ifndef INCLUDE_TELEGRAM_TELEGRAMOBJECT_H_
#define INCLUDE_TELEGRAM_TELEGRAMOBJECT_H_

#include "Telegram.h"
#include "../EventSystemParticipant.h"

namespace EventSystem
{
/*
class Telegram1 {
public:
	//enum telegram_type {ANONYMOUS, LOG, REGISTER, UNREGISTER, REQUEST, INPUT, DISPLAYDIMENSION, MEDIA, DISPLAYDATA, PING, QUIT};
	static const uint8 ANONYMOUS;
	static const uint8 LOG;
	static const uint8 REGISTER;
	static const uint8 UNREGISTER;
	static const uint8 REQUEST;
	static const uint8 REQUESTANSWER;
	static const uint8 PING;
	static const uint8 INPUT;
	static const uint8 DISPLAYDATA;
	static const uint8 MEDIA;
	static const uint8 QUIT;

    Telegram1(std::string identifier);
    Telegram1();
    virtual ~Telegram1();

    //TODO: static function init(EventSystemParticipant* esp), automatically sets source

    void setUniqueDestination(bool set);
    bool isUniqueDestination();

    static std::string ID_LOGGER;
    static std::string ID_MASTER;
    static std::string ID_DISPLAY;
    static std::string ID_DISPLAYCLIENT;
    static std::string ID_AUDIOPLAYER;
    static std::string ID_INPUT;

    void setIdentifier(std::string identifier);
    void setSource(std::string source);

    char* getDestinationID();
    char* getSourceID();
    int getSize();

    virtual int getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);

	uint8 getType();
	void setType(uint8 type);

	static void initTelegram(EventSystemParticipant* esp);

protected:
	int8 destinationID[UNIQUEID_SIZE];
	int8 sourceID[UNIQUEID_SIZE];
	uint8 type;
	int8 uniqueDestination;
    int32 telegramSize;
};
*/

class Telegram_Object : public Telegram {
public:
	Telegram_Object(std::string destination, Serializeable* obj);
	Telegram_Object();
	virtual ~Telegram_Object();

    virtual int getSerializedSize();
	virtual int serialize(void* const data);
	virtual int deserialize(void const * const data);
	virtual int deserialize(void const * const data, Serializeable* obj);

	Serializeable* getObject();
	void setObject(Serializeable* obj);

private:
	Serializeable* object;
};

} /* namespace EventSystem */

#endif /* INCLUDE_TELEGRAM_TELEGRAMOBJECT_H_ */
