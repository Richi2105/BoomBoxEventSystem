#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <stdint.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include <OS_DEF.h>

#include "../EventSystemParticipant.h"
#include "../constants.h"
#include "../Serializeable.h"

#define TELEGRAM_ANONYMOUS 0
#define TELEGRAM_LOG 1
#define TELEGRAM_REGISTER 2
#define TELEGRAM_UNREGISTER 3
#define TELEGRAM_REQUEST 4
#define TELEGRAM_REQUESTANSWER 5
#define TELEGRAM_PING 6
#define TELEGRAM_INPUT 7
#define TELEGRAM_DISPLAYDATA 8
#define TELEGRAM_MEDIA 9
#define TELEGRAM_QUIT 254


namespace EventSystem
{

class Telegram : public Serializeable
{
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

        Telegram(std::string identifier);
        Telegram();
        virtual ~Telegram();

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
        //int32 telegramSize;

        static EventSystemParticipant* esp;
};

} /* namespace EventSystem */

#endif // TELEGRAM_H
