#include "../include/Telegram/Telegram_Display.h"

Telegram_Display::Telegram_Display(DisplayPosition pos, std::string dispstr) : Telegram("DISPLAY")
{
    memcpy(this->str, dispstr.c_str(), dispstr.size());
    this->pos = pos;
    this->telegramSize += STRINGSIZE + sizeof(DisplayPosition);

}

Telegram_Display::~Telegram_Display()
{

}

DisplayPosition Telegram_Display::getPosition()
{
    return this->pos;
}

char* Telegram_Display::getDisplayString()
{
    return this->str;
}
