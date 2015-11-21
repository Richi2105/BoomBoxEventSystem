#ifndef TELEGRAM_DISPLAY_H
#define TELEGRAM_DISPLAY_H

#include <string>

#include "Telegram.h"
#include "Display/DisplayPosition.h"

#define STRINGSIZE 22


class Telegram_Display : public Telegram
{
    public:
        Telegram_Display(DisplayPosition pos, std::string dispstr);
        virtual ~Telegram_Display();
        DisplayPosition getPosition();
        char* getDisplayString();
    protected:
    private:
        DisplayPosition pos;
        char str[STRINGSIZE];
};

#endif // TELEGRAM_DISPLAY_H
