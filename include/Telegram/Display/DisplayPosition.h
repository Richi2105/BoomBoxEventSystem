#ifndef DISPLAYPOSITION_H
#define DISPLAYPOSITION_H


class DisplayPosition
{
    public:
        DisplayPosition(int x, int y);
        DisplayPosition();
        virtual ~DisplayPosition();
        int getXPosition();
        int getYPosition();
    protected:
    private:
        int xPosition;
        int yPosition;
};

#endif // DISPLAYPOSITION_H
