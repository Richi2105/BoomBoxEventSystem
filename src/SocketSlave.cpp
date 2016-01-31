#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/Telegram/Telegram.h"

#include "../include/Register/RegisterLocal.h"
#include "../include/Telegram/TelegramObject.h"

#include "../include/SocketIOLocal.h"
#include "../include/SocketIONetwork.h"

#include "../include/SocketAddress.h"
#include "../include/SocketSlave.h"

Socket_Slave::Socket_Slave()
{
	printf("Creating Socket_Slave()\n");
    this->socket = new SocketIO_Local();
//    this->masterAddress = new SocketAddressLocal();
    this->getAddressFromSharedMemory(((SocketAddressLocal**)&this->masterAddress));
    printf("Master Address: %s\n", ((sockaddr_un*)((SocketAddressLocal*)this->masterAddress)->getAddress())->sun_path);
    this->local = true;
}

Socket_Slave::Socket_Slave(in_port_t port, char* networkDevice)
{
	printf("Creating Socket_Slave(%d)\n", port);
	this->socket = new SocketIO_Network(port, NULL);
	this->masterAddress = new SocketAddressNetwork();
	this->local = false;
}

Socket_Slave::~Socket_Slave()
{
    delete this->socket;
    delete this->masterAddress;
}

int Socket_Slave::send(void* data, int numOfBytes)
{

#ifdef DEBUG_OUT
	unsigned char* data1 = (unsigned char*) data;
	printf("Contents of Telegram\n");
	for (int i=0; i<numOfBytes; i+=1)
	{
		if ((i) % 8==0)
		{
			printf("\t(%d:) ", i);
		}

		printf("%2x ", *(data1+i));

		if ((i+1) % 16==0)
		{
			printf("\n");
		}
	}
#endif //DEBUG_OUT
    return sendto(this->socket->getSocketFileDescriptor(), data, numOfBytes, 0, this->masterAddress->getAddress(), this->masterAddress->getLen());
}

int Socket_Slave::receive(void* data, int numOfBytes)
{
	int bytes = this->socket->receive(data, numOfBytes);
#ifdef DEBUG_OUT
	unsigned char* data1 = (unsigned char*) data;
	printf("Contents of Telegram\n");
	for (int i=0; i<bytes; i+=1)
	{
		if ((i) % 8==0)
		{
			printf("\t(%d:) ", i);
		}

		printf("%2x ", *(data1+i));

		if ((i+1) % 16==0)
		{
			printf("\n");
		}

	}
#endif //DEBUG_OUT
	return bytes;
}

std::string Socket_Slave::getUniqueID()
{
	return this->socket->getUniqueID();
}

void Socket_Slave::setAddress(struct sockaddr_in address, socklen_t len)
{
	if (!this->local)
	{
		((SocketAddressNetwork*)this->masterAddress)->setAddress(address, len);
	}
}

bool Socket_Slave::isLocal()
{
	return this->local;
}

SocketAddress* Socket_Slave::getAddress()
{
	return this->socket->getAddress();
}

SocketIO* Socket_Slave::getSocket()
{
	return this->socket;
}

void Socket_Slave::getAddressFromSharedMemory(SocketAddressLocal** address)
{
	int fd;
#ifdef RASPBERRY
	fd = shmget((key_t)4321, sizeof(SocketAddressLocal), 0444);
#else
    fd = shm_open("/shm_EventSystemSHM", O_RDWR, S_IRUSR | S_IWUSR);
#endif //RASPBERRY
	if (fd == -1)
	{
		fprintf(stderr, "Error: shm_open()\n");
		exit(1);
	}
	printf("File Descriptor to /shm_EventSystemSHM: %d\n", fd);

#ifdef RASPBERRY
	void* mapAddress = shmat(fd, NULL, 0);
#else
	void* mapAddress = mmap(NULL, sizeof(SocketAddressLocal), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
#endif //RASPBERRY

	if (mapAddress == MAP_FAILED || mapAddress == NULL)
	{
		fprintf(stderr, "Error: mmap()\n");
		exit(1);
	}
	printf("mapAddress: %p\n", mapAddress);

	(*address) = new SocketAddressLocal(*(SocketAddressLocal*) mapAddress);

	printf("%s\n", ((sockaddr_un*)(*address)->getAddress())->sun_path);

#ifdef RASPBERRY
	shmdt(mapAddress);
#else
	munmap(mapAddress, sizeof(SocketAddressLocal));
#endif //RASPBERRY

	close(fd);
}
