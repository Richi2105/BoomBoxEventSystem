#ifndef SOCKETADDRESS_H_INCLUDED
#define SOCKETADDRESS_H_INCLUDED

#include <sys/socket.h>
#include <stdint.h>
//Interface
class SocketAddress
{
    public:
		virtual ~SocketAddress() = 0;

		/**
		 * @return: a struct pointer to a sockaddr structure. It is either a sockaddr_in or sockaddr_un struct,
		 * depending on the return value of isLocal().
		 * @see isLocal()
		 */
        virtual sockaddr* getAddress() = 0;

        /**
         * @return the length of the sockaddr structure depending on the system and if it is a local address
         * or a network address.
         */
        virtual socklen_t getLen() = 0;

        /**
         * @deprecated
         */
        virtual uint8_t getSize() = 0;

        /**
         * @return: the unique id set by the correspondent SocketIO object.
         * Warning: this is not what the EventSystemClient would return, it adds it's name to it.
         * To fix this issue, call setUID(EventSystemClient::getUniqueID()) in the proper SocketAddress object.
         */
        virtual char* getUniqueID() = 0;

        /**
         * @return: whether the object is a local address (true) or a network address (false)
         */
        virtual bool isLocal() = 0;

        /**
         * checks if another address has the same attributes:
         * 	-same unique name
         * 	-same sockaddr (sockaddr_un.sun_address or sockaddr_in.sin_address value)
         * @return: false if it is not equal, true if it is equal.
         */
        virtual bool isEqual(SocketAddress*) = 0;
        virtual bool operator==(SocketAddress*) = 0;
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
