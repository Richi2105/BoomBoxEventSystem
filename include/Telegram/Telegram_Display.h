#ifndef TELEGRAM_DISPLAY_H
#define TELEGRAM_DISPLAY_H

#include <string>

#include "Telegram.h"
#include "/home/richard/workspace/DotMatrixDisplayLib/include/DisplayPosition.h"
//#include "Display/DisplayPosition.h"

#define STRINGSIZE 22


class Telegram_Display : public EventSystem::Telegram
{
    public:
        Telegram_Display(DisplayPosition pos, std::string dispstr);
        Telegram_Display();
        virtual ~Telegram_Display();
        DisplayPosition getPosition();
        char* getDisplayString();

        virtual int16_t getSerializedSize();
    	virtual int serialize(void* const data);
    	virtual int deserialize(void const * const data);

    protected:
    private:
        DisplayPosition pos;	//todo: abstract object in telegram
        char str[STRINGSIZE];
};

#endif // TELEGRAM_DISPLAY_H
