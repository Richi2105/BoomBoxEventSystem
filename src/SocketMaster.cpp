#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/SocketAddress.h"
#include "../include/SocketMaster.h"

Socket_Master::Socket_Master(in_port_t port)
{
    this->localSocket = new SocketIO_Local();
    this->networkSocket = new SocketIO_Network(port);
    this->mapAddress = NULL;
}

Socket_Master::~Socket_Master()
{
    //dtor
}

std::string Socket_Master::getUniqueID()
{
	return localSocket->getUniqueID();
}

void Socket_Master::broadcast()
{
    printf("broadcasting...\n");
    int fd;
#ifdef RASPBERRY
    fd = shmget((key_t) 4321, sizeof(SocketAddressLocal), 0666 | IPC_CREAT);
#else
    fd = shm_open("/shm_EventSystemSHM", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
#endif //RASPBERRY

	if (fd == -1)
	{
		fprintf(stderr, "Error: shm_open()\n");
		exit(1);
	}
	printf("File Descriptor to /shm_EventSystemSHM: %d\n", fd);
	ftruncate(fd, sizeof(SocketAddressLocal));

#ifdef RASPBERRY
	this->mapAddress = shmat(fd, NULL, 0);
#else
	this->mapAddress = mmap(NULL, sizeof(SocketAddressLocal), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
#endif //RASPBERRY

	if (mapAddress == MAP_FAILED || mapAddress == NULL)
    {
        fprintf(stderr, "Error: mmap()\n");
		exit(1);
    }

#ifndef RASPBERRY
    memset(mapAddress, 0, sizeof(SocketAddressLocal));
#endif //RASPBERRY


    SocketAddressLocal* esaddress = (SocketAddressLocal*) mapAddress;
    struct sockaddr_un* sockaddr = (sockaddr_un*) ((this->localSocket->getAddress()->getAddress()));

    esaddress->setAddress(*sockaddr, this->localSocket->getAddress()->getLen());

//    printf("path to me: %s, with len %d\n", ((sockaddr_un*)((SocketAddressLocal*)mapAddress)->getAddress())->sun_path, esaddress->getLen());

    close(fd);
}

SocketIO_Local* Socket_Master::getLocalSocket()
{
	return this->localSocket;
}

SocketIO_Network* Socket_Master::getNetworkSocket()
{
	return this->networkSocket;
}

int Socket_Master::send(void* data, int numOfBytes, SocketAddressLocal* dest)
{
    printf("Sending to: %s\n", ((sockaddr_un*)dest->getAddress())->sun_path);
    return sendto(this->localSocket->getSocketFileDescriptor(), data, numOfBytes, 0, dest->getAddress(), dest->getLen());
}

int Socket_Master::send(void* data, int numOfBytes, SocketAddressNetwork* dest)
{
    printf("Sending to: %d\n", ((sockaddr_in*)dest->getAddress())->sin_addr.s_addr);
    return sendto(this->networkSocket->getSocketFileDescriptor(), data, numOfBytes, 0, dest->getAddress(), dest->getLen());
}
