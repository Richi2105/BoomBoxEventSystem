#include "DisplayPosition.h"

DisplayPosition::DisplayPosition(int x, int y)
{
    this->xPosition = x;
    this->yPosition = y;
}

DisplayPosition::DisplayPosition()
{
    this->xPosition = 0;
    this->yPosition = 0;
}

DisplayPosition::~DisplayPosition()
{
    //dtor
}

int DisplayPosition::getXPosition()
{
    return this->xPosition;
}

int DisplayPosition::getYPosition()
{
    return this->yPosition;
}
