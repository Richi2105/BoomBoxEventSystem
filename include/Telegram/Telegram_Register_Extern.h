#ifndef TELEGRAM_REGISTER_EXTERN_H
#define TELEGRAM_REGISTER_EXTERN_H

#include <netinet/ip.h>
#include <string>
#include <string.h>
#include <stdio.h>

#include "../SocketAddressNetwork.h"


#include "Telegram.h"


class Telegram_Register_Extern : public Telegram
{
    public:
        Telegram_Register_Extern(SocketAddressNetwork clientAddress, std::string id);
        virtual ~Telegram_Register_Extern();
        SocketAddressNetwork* getClientAddress();
        char* getClientID();
    protected:
    private:
        char clientID[ID_SIZE];
        SocketAddressNetwork clientAddress;

};

#endif // TELEGRAM_REGISTER_EXTERN_H
