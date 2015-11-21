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
#include "../include/Telegram/Telegram_Register.h"
#include "../include/Telegram/Telegram_Register_Extern.h"

#include "../include/SocketIOLocal.h"
#include "../include/SocketIONetwork.h"

#include "../include/SocketAddress.h"
#include "../include/SocketSlave.h"

Socket_Slave::Socket_Slave()
{
    this->socket = new SocketIO_Local();
    this->masterAddress = new SocketAddressLocal();
    this->getAddressFromSharedMemory((SocketAddressLocal*)this->masterAddress);
    this->local = true;
}

Socket_Slave::Socket_Slave(in_port_t port)
{
	this->socket = new SocketIO_Network(port);
	this->masterAddress = new SocketAddressNetwork();
	this->local = false;
}

Socket_Slave::~Socket_Slave()
{
    //dtor
}

int Socket_Slave::send(void* data, int numOfBytes)
{
    return sendto(this->socket->getSocketFileDescriptor(), data, numOfBytes, 0, this->masterAddress->getAddress(), this->masterAddress->getLen());
}

void Socket_Slave::setAddress(struct sockaddr_in address, socklen_t len)
{
	((SocketAddressNetwork*)this->masterAddress)->setAddress(address, len);
}

SocketAddress* Socket_Slave::getAddress()
{
	return this->socket->getAddress();
}

SocketIO* Socket_Slave::getSocket()
{
	return this->socket;
}

void Socket_Slave::connect(std::string id)
{
    printf("connecting...\n");
    if (this->masterAddress->getAddress() == nullptr)
    {
    	printf("nothing to connect to, set Address first\n");
    }
    else
    {
    	Telegram* telegram;
    	if (this->local)
    	{
			telegram = new Telegram_Register(*(SocketAddressLocal*)this->socket->getAddress(), id);
			printf("Telegram initialized, source is %s\n", ((Telegram_Register*)telegram)->getClientID());
    	}
    	else
    	{
    		telegram = new Telegram_Register_Extern(*(SocketAddressNetwork*)this->socket->getAddress(), id);
    		printf("Telegram initialized, source is %s\n", ((Telegram_Register*)telegram)->getClientID());
    	}

    	this->send((void*) telegram, telegram->getSize());
    }
}

void Socket_Slave::getAddressFromSharedMemory(SocketAddressLocal* address)
{
	int fd;
	fd = shmget((key_t)4321, sizeof(SocketAddressLocal), 0444);
//    fd = shm_open("/shm_EventSystemSHM", O_RDONLY, S_IRUSR | S_IWUSR);
	if (fd == -1)
	{
		fprintf(stderr, "Error: shm_open()\n");
		exit(1);
	}
	printf("File Descriptor to /shm_EventSystemSHM: %d\n", fd);

//	void* mapAddress = mmap(NULL, sizeof(SocketAddressLocal), PROT_READ, MAP_SHARED, fd, 0);
	void * mapAddress = shmat(fd, NULL, 0);
	if (mapAddress == MAP_FAILED || mapAddress == NULL)
	{
		fprintf(stderr, "Error: mmap()\n");
		exit(1);
	}

	SocketAddressLocal* esaddress = (SocketAddressLocal*) mapAddress;
	address->setAddress((*(sockaddr_un*)esaddress->getAddress()), esaddress->getLen());

	printf("connected to: %s, with len %d\n", ((sockaddr_un*)((SocketAddressLocal*)mapAddress)->getAddress())->sun_path, ((SocketAddressLocal*)mapAddress)->getLen());

	close(fd);
}
