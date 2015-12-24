#ifndef TELEGRAM_REGISTER_H
#define TELEGRAM_REGISTER_H

#include <sys/un.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "../SocketAddressLocal.h"

#include "Telegram.h"


class Telegram_Register : public EventSystem::Telegram
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

#endif // TELEGRAM_REGISTER_H
