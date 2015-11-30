#ifndef TELEGRAM_REGISTER_H
#define TELEGRAM_REGISTER_H

#include <sys/un.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "../SocketAddressLocal.h"

#include "Telegram.h"


class Telegram_Register : public Telegram
{
    public:
        Telegram_Register(SocketAddressLocal clientAddress, std::string id);
        Telegram_Register();
        virtual ~Telegram_Register();
        SocketAddressLocal* getClientAddress();
        char* getClientID();

        virtual int16_t getSerializedSize();
    	virtual int serialize(void* const data);
    	virtual int deserialize(void const * const data);

    protected:
    private:
        char clientID[ID_SIZE];
        SocketAddressLocal clientAddress;

};

struct _telegram_register_local{
	telegram_head header;
	int8_t clientID[ID_SIZE];
	socketAddress_local address;
};

typedef struct _telegram_register_local telegram_register_local;

void initTelegram_Register_Local(telegram_register_local* telegram, socketAddress_local* address, std::string* clientID);

#endif // TELEGRAM_REGISTER_H
