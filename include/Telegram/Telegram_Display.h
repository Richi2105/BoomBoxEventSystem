#ifndef TELEGRAM_DISPLAY_H
#define TELEGRAM_DISPLAY_H

#include <string>

#include "Telegram.h"
#include "/home/richard/workspace/DotMatrixDisplayLib/include/DisplayPosition.h"
//#include "Display/DisplayPosition.h"

#define STRINGSIZE 22


class Telegram_Display : public Telegram
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

struct _telegram_display{
	telegram_head header;
	int8_t displayString[STRINGSIZE];
	uint8_t xpos;
	uint8_t ypos;
};

typedef struct _telegram_display telegram_display;

void initTelegram_Display(telegram_display* telegram, DisplayPosition* pos, std::string* dispstr);

#endif // TELEGRAM_DISPLAY_H
