#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <stdint.h>
#include <string>
#include <string.h>

#define ID_SIZE 20

class Telegram
{
    public:
        Telegram(std::string identifier);
        virtual ~Telegram();
        char* getDestinationID();
        int getSize();
    protected:
        char destinationID[ID_SIZE];
        int telegramSize;
};

#endif // TELEGRAM_H
