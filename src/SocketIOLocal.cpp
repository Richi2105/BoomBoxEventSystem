#include <sys/socket.h>
#include <sys/un.h>


#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/SocketIOLocal.h"


SocketIO_Local::SocketIO_Local() : myAddress()
{
	printf("Creating SocketIO_Local\n");
    char uid[25];
    snprintf(uid, 25, "UID=%ld", (long) getpid());
    this->uniqueID = uid;

    struct sockaddr_un mySockAddress;
    socklen_t socklen;

//    this->pMyAddress = &this->myAddress;
    this->socketFileDescriptor = socket(AF_UNIX, SOCK_DGRAM, 0);
    socklen = 64;
    memset(&mySockAddress, 0, sizeof(struct sockaddr_un));
    mySockAddress.sun_family = AF_UNIX;
    snprintf(mySockAddress.sun_path, sizeof(mySockAddress.sun_path), "/tmp/ipc_%ld_%d", (long) getpid(), rand());

    bind(this->socketFileDescriptor, (struct sockaddr*)&mySockAddress, sizeof(sockaddr_un));
//    getsockname(this->socketFileDescriptor, (struct sockaddr*)&mySockAddress, &socklen);
    printf("my Address: %s\n", mySockAddress.sun_path);
    this->myAddress.setAddress(mySockAddress, socklen);
}

SocketIO_Local::~SocketIO_Local()
{
    close(this->socketFileDescriptor);
}

std::string SocketIO_Local::getUniqueID()
{
    return uniqueID;
}

SocketAddress* SocketIO_Local::getAddress()
{
    return ((SocketAddress*) &this->myAddress);
}

int SocketIO_Local::getSocketFileDescriptor()
{
	return this->socketFileDescriptor;
}

int SocketIO_Local::send(void* data, int numOfBytes)
{
    return 0;
}

int SocketIO_Local::receive(void* data, int numOfBytes)
{
    return recvfrom(this->socketFileDescriptor, data, numOfBytes, 0, NULL, NULL);
}
