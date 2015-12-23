#ifndef EVENTSYSTEMPARTICIPANT_H_INCLUDED
#define EVENTSYSTEMPARTICIPANT_H_INCLUDED

#include <string>
#include <sys/un.h>

#include "SocketIOLocal.h"
#include "SocketAddress.h"

#include "Telegram/TelegramObject.h"

class EventSystemParticipant
{
    public:
		virtual ~EventSystemParticipant() = 0;
        virtual std::string getIdentifier() = 0;
        virtual std::string getUniqueIdentifier() = 0;
        virtual SocketAddress* getAddress() = 0;
        virtual SocketIO* getSocket() = 0;
        virtual void log(Telegram::Telegram_Object* log) = 0;

//        virtual int receive(bool nonblocking, Telegram* telegram) = 0;
//        virtual int send(Telegram* telegram) = 0;

    protected:
    private:

};



#endif // EVENTSYSTEMPARTICIPANT_H_INCLUDED
