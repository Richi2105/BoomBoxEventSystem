#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <stdint.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include "../Serializeable.h"

namespace Telegram
{

#define ID_SIZE 20



class Telegram : public Serializeable
{
    public:
		enum telegram_type {ANONYMOUS, LOG, REGISTER, UNREGISTER, REQUEST, INPUT, DISPLAYDIMENSION, MEDIA, DISPLAYDATA};

        Telegram(std::string identifier);
        virtual ~Telegram();
        char* getDestinationID();
        int getSize();


        virtual int16_t getSerializedSize();
    	virtual int serialize(void* const data);
    	virtual int deserialize(void const * const data);

    	telegram_type getType();
    	void setType(telegram_type type);

    protected:
		char destinationID[ID_SIZE];
		telegram_type type;
        int telegramSize;
};

} /* namespace Telegram */



struct _telegram{
	int8_t destinationID[ID_SIZE];
	int16_t telegramSize;
};

typedef struct _telegram telegram_head;

void initTelegram(telegram_head* telegram, std::string* id);

#endif // TELEGRAM_H
