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

		/* anonymous type */
		static const uint8 ANONYMOUS;

		/* a log is inside */
		static const uint8 LOG;

		/* Client wants to register to the master */
		static const uint8 REGISTER;

		/* Client wants to unregister from the master */
		static const uint8 UNREGISTER;

		/* request of special data of a module */
		static const uint8 REQUEST;

		/* the answer to a request */
		static const uint8 REQUESTANSWER;

		/* ping from Master */
		static const uint8 PING;

		/* a pressed Key is inside */
		static const uint8 INPUT;

		/* a display object is inside */
		static const uint8 DISPLAYDATA;

		 /* ??? */
		static const uint8 MEDIA;

		/* ??? */
		static const uint8 QUIT;

        Telegram(std::string identifier);
        Telegram();
        virtual ~Telegram();

        /**
         * set the destination to be unique, i.e. send to a single client
         * @param set: true for unique destination, false for all clients matching the ID
         * make sure the ID is a unique ID
         */
        void setUniqueDestination(bool set);

        /**
         * @return if the telegram is meant to be sent to a single client
         */
        bool isUniqueDestination();

        static std::string ID_LOGGER;
        static std::string ID_MASTER;
        static std::string ID_DISPLAY;
        static std::string ID_DISPLAYCLIENT;
        static std::string ID_AUDIOPLAYER;
        static std::string ID_INPUT;

        /**
         * set the identifier (i.e. the destination)
         * @param identifier: can be an ID or an Unique ID
         */
        void setIdentifier(std::string identifier);

        /**
         * set the source of this telegram
         * @param source: should be an Unique ID
         */
        void setSource(std::string source);

        /**
         * @return the destination ID
         */
        char* getDestinationID();

        /**
         * @return the source ID
         */
        char* getSourceID();

        /**
         * @DEPRECATED
         */
        int getSize();

        virtual int getSerializedSize();
    	virtual int serialize(void* const data);
    	virtual int deserialize(void const * const data);

    	/**
    	 * Types:
    	 * identify the nature of a telegram, so a client can determine what object might be inside
    	 * @return the type of this telegram
    	 */
    	uint8 getType();

    	/**
    	 * set the type of this telegram
    	 * @param type the type duh
    	 */
    	void setType(uint8 type);

    	/**
    	 * this function binds all new initialized telegram instances (and those derived) to the specified Participant.
    	 * This means that all telegrams source ID are automatically set to the UID of the participant.
    	 * @param esp: the reference to the EventSystemParticipant
    	 */
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
