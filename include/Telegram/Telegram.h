#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <stdint.h>
#include <string>
#include <string.h>
#include <stdint.h>

#define ID_SIZE 20

class Telegram
{
    public:
        Telegram(std::string identifier);
        virtual ~Telegram();
        char* getDestinationID();
        int getSize();

        char destinationID[ID_SIZE];
    protected:

        int telegramSize;
};



struct _telegram{
	int8_t destinationID[ID_SIZE];
	int16_t telegramSize = 0;
};

typedef struct _telegram telegram;

void initTelegram(telegram* telegram, std::string id);

#endif // TELEGRAM_H
