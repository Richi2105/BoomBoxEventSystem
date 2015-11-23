#ifndef TELEGRAM_REGISTER_H
#define TELEGRAM_REGISTER_H

#include <sys/un.h>
#include <string>
#include <string.h>
#include <stdio.h>

#include "../SocketAddressLocal.h"

#include "Telegram.h"


class Telegram_Register : public Telegram
{
    public:
        Telegram_Register(SocketAddressLocal clientAddress, std::string id);
        virtual ~Telegram_Register();
        SocketAddressLocal* getClientAddress();
        char* getClientID();
    protected:
    private:
        char clientID[ID_SIZE];
        SocketAddressLocal clientAddress;

};

#endif // TELEGRAM_REGISTER_H
