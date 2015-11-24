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

struct _telegram_register_network{
	telegram header;
	int8_t clientID[ID_SIZE];
	socketAddress_network address;
};

typedef struct _telegram_register_network telegram_register_network;

void initTelegram_Register_Network(telegram_register_network* telegram, socketAddress_network address, std::string clientID);

#endif // TELEGRAM_REGISTER_EXTERN_H
