#ifndef EVENTSYSTEMCLIENT_H
#define EVENTSYSTEMCLIENT_H


class EventSystemClient
{
    public:
        EventSystemClient();
        virtual ~EventSystemClient();
    protected:
    private:
        SocketAddress* address;
};

#endif // EVENTSYSTEMCLIENT_H
