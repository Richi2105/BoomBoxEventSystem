#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <stdint.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include <OS_DEF.h>
#include "../constants.h"
#include "../Serializeable.h"

//TODO: factory for all kinds of ObjectTelegrams

namespace EventSystem
{

class Telegram : public Serializeable
{
    public:
		enum telegram_type {ANONYMOUS, LOG, REGISTER, UNREGISTER, REQUEST, INPUT, DISPLAYDIMENSION, MEDIA, DISPLAYDATA};
		//TODO: telegram_type should be struct/class which can be extended by values
		/**
		 * class telegram_type{
		 * 	public:
		 * 	int32 type;
		 * 	const static int32 ... = ...;
		 * }
		 */

        Telegram(std::string identifier, std::string source = "");
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

    	telegram_type getType();
    	void setType(telegram_type type);

    protected:
		int8 destinationID[UNIQUEID_SIZE];
		int8 sourceID[UNIQUEID_SIZE];
		telegram_type type;
		int8 uniqueDestination;
        int32 telegramSize;
};

} /* namespace EventSystem */

#endif // TELEGRAM_H
