#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <stdint.h>
#include <string>
#include <string.h>
#include <stdint.h>
#include "../Serializeable.h"

#define ID_SIZE 20

enum telegram_type {ANONYMOUS, LOG, REGISTER, INPUT, REQUEST, DISPLAYDIMENSION, MEDIA, DISPLAYDATA};

class Telegram : public Serializeable
{
    public:


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



struct _telegram{
	int8_t destinationID[ID_SIZE];
	int16_t telegramSize = 0;
};

typedef struct _telegram telegram_head;

void initTelegram(telegram_head* telegram, std::string* id);

#endif // TELEGRAM_H
