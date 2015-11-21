#ifndef SOCKETADDRESS_H_INCLUDED
#define SOCKETADDRESS_H_INCLUDED

#include <sys/socket.h>
#include <stdint.h>
//Interface
class SocketAddress
{
    public:
		virtual ~SocketAddress() = 0;
        virtual sockaddr* getAddress() = 0;
        virtual socklen_t getLen() = 0;
        virtual uint8_t getSize() = 0;

};
/*
//typedef class SocketAddress SockAddress;

template <typename __sockaddr_t>
class SocketAddress
{
    public:
        SocketAddress(__sockaddr_t address, socklen_t len);
        SocketAddress();
        virtual ~SocketAddress();

        void setAddress(__sockaddr_t address, socklen_t len);

        __sockaddr_t* getAddress();
        socklen_t getLen();
        uint8_t getSize();

    private:
        __sockaddr_t address;
        socklen_t len;
        uint8_t addressSize;

};

template <typename __sockaddr_t>
class SocketAddress;

template <typename __sockaddr_t>
SocketAddress<__sockaddr_t>::SocketAddress(__sockaddr_t address, socklen_t len)
{
    this->address = address;
    this->len = len;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(__sockaddr_t);
}

template <typename __sockaddr_t>
SocketAddress<__sockaddr_t>::SocketAddress()
{
    memset(&(this->address), 0, sizeof(__sockaddr_t));
    this->len = 0;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(__sockaddr_t);
}

template <typename __sockaddr_t>
SocketAddress<__sockaddr_t>::~SocketAddress(){}

template <typename __sockaddr_t>
void SocketAddress<__sockaddr_t>::setAddress(__sockaddr_t address, socklen_t len)
{
    this->address = address;
    this->len = len;
    this->addressSize = sizeof(int8_t) + sizeof(socklen_t) + sizeof(__sockaddr_t);
}

template <typename __sockaddr_t>
__sockaddr_t* SocketAddress<__sockaddr_t>::getAddress()
{
    return &(this->address);
}

template <typename __sockaddr_t>
socklen_t SocketAddress<__sockaddr_t>::getLen()
{
    return this->len;
}

template <typename __sockaddr_t>
uint8_t SocketAddress<__sockaddr_t>::getSize()
{
    return this->addressSize;
}
*/

#endif // SOCKETADDRESS_H_INCLUDED
