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
	printf("Creating Socket_Slave()\n");
    this->socket = new SocketIO_Local();
    this->masterAddress = new SocketAddressLocal();
    this->getAddressFromSharedMemory(((SocketAddressLocal**)&this->masterAddress));
    printf("Master Address: %s\n", ((sockaddr_un*)((SocketAddressLocal*)this->masterAddress)->getAddress())->sun_path);
    this->local = true;
}

Socket_Slave::Socket_Slave(in_port_t port)
{
	printf("Creating Socket_Slave(%d)\n", port);
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
	unsigned char* data1 = (unsigned char*) data;
	printf("Contents of Telegram\n");
	for (int i=0; i<numOfBytes; i+=1)
	{
		printf("%2x ", *(data1+i));
		if (i%16==0)
		{
			printf("\n");
		}

	}
    return sendto(this->socket->getSocketFileDescriptor(), data, numOfBytes, 0, this->masterAddress->getAddress(), this->masterAddress->getLen());
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
    	void* data;
//    	void* telegram = malloc(sizeof(telegram_register_network));
//    	memset(telegram, 0, sizeof(telegram_register_network));
    	if (this->local)
    	{
			telegram = new Telegram_Register(*(SocketAddressLocal*)this->socket->getAddress(), id);
			printf("telegram initialized\n");
			data = malloc(telegram->getSerializedSize());
			printf("data allocated\n");
			((Telegram_Register*)telegram)->serialize(data);
			printf("Telegram initialized, source is %s\n", ((Telegram_Register*)telegram)->getClientID());
//    		socketAddress_local addr;
//    		this->masterAddress->convertTo_Struct((void*) &addr);
//    		initTelegram_Register_Local((telegram_register_local*) telegram, &addr,	&id);
//    		printf("Telegram initialized, source is %s", ((telegram_head*)telegram)->destinationID);
    	}
    	else
    	{
    		telegram = new Telegram_Register_Extern(*(SocketAddressNetwork*)this->socket->getAddress(), id);
    		data = malloc(telegram->getSerializedSize());
    		((Telegram_Register_Extern*)telegram)->serialize(data);
    		printf("Telegram initialized, source is %s\n", ((Telegram_Register_Extern*)telegram)->getClientID());
//    		socketAddress_network addr;
//    		this->masterAddress->convertTo_Struct((void*) &addr);
//    		initTelegram_Register_Network((telegram_register_network*) telegram, &addr,	&id);
//    		printf("Telegram initialized, source is %s", ((telegram_head*)telegram)->destinationID);
    	}

    	this->send(data, telegram->getSerializedSize());
    }
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
	//memcpy(address, mapAddress, sizeof(SocketAddressLocal));
	//sockaddr* sockaddr = esaddress->getAddress();
	//address->setAddress(sockaddr, esaddress->getLen());

//	printf("connected to: %s, with len %d\n", ((sockaddr_un*)((SocketAddressLocal*)mapAddress)->getAddress())->sun_path, ((SocketAddressLocal*)mapAddress)->getLen());

	close(fd);
}
