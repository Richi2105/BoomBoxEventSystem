#include <sys/socket.h>
#include <sys/un.h>

#include <time.h>
#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/SocketIOLocal.h"

SocketIO_Local::SocketIO_Local()
{
	printf("Creating SocketIO_Local\n");

    srand((int) getpid() + (int) time(NULL));
    int randomNr = rand();
    for (int i=0; i<randomNr%5; i+=1)
    	rand();
    randomNr = rand() % 10000;

    char path[22];
    snprintf(path, 22, "%08x:%04d", getpid(), randomNr);

    char uid[25];
    snprintf(uid, 25, "%s", path);
    this->uniqueID = uid;

    struct sockaddr_un mySockAddress;
    socklen_t socklen;

//    this->pMyAddress = &this->myAddress;
    this->socketFileDescriptor = socket(AF_UNIX, SOCK_DGRAM, 0);
    socklen = 64;
    memset(&mySockAddress, 0, sizeof(struct sockaddr_un));
    mySockAddress.sun_family = AF_UNIX;

    snprintf(mySockAddress.sun_path, sizeof(mySockAddress.sun_path), "/tmp/ipc_%s", path);

    bind(this->socketFileDescriptor, (struct sockaddr*)&mySockAddress, sizeof(sockaddr_un));
//    getsockname(this->socketFileDescriptor, (struct sockaddr*)&mySockAddress, &socklen);
    printf("my Address: %s\n", mySockAddress.sun_path);
    this->myAddress = new SocketAddressLocal(mySockAddress, socklen, this->uniqueID);
}

SocketIO_Local::~SocketIO_Local()
{
	close(this->socketFileDescriptor);
	unlink(((struct sockaddr_un*)this->myAddress->getAddress())->sun_path);
	delete this->myAddress;
}

std::string SocketIO_Local::getUniqueID()
{
    return this->uniqueID;
}

SocketAddress* SocketIO_Local::getAddress()
{
    return ((SocketAddress*) this->myAddress);
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
