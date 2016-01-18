/*
 * SocketReaderNetwork.cpp
 *
 *  Created on: Nov 20, 2015
 *      Author: richard
 */

#include "../include/SocketIONetwork.h"

#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#include <fcntl.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


SocketIO_Network::SocketIO_Network(in_port_t port, char* device)
{
	printf("Creating SocketIO_Network(%d)\n", port);
	char uid[25];
	snprintf(uid, 25, "UID=%ld", (long) getpid());
	this->uniqueID = uid;
	char address[INET_ADDRSTRLEN];

	struct sockaddr_in mySockAddress;
	socklen_t socklen;

	this->socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
	socklen = 64;
	memset(&mySockAddress, 0, sizeof(struct sockaddr_in));
	mySockAddress.sin_family = AF_INET;
	mySockAddress.sin_port = port;

	struct ifaddrs * ifAddrStruct=NULL;
	struct ifaddrs * ifa=NULL;
	void * tmpAddrPtr=NULL;

	getifaddrs(&ifAddrStruct);

	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (!ifa->ifa_addr) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET)
		{ // check it is IP4
			// is a valid IP4 Address
			tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
			if (device == nullptr)
			{
				if (!strncmp(addressBuffer, "192.168.", 8))
				{
					mySockAddress.sin_addr = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
					printf("Address saved\n");
					memcpy(address, addressBuffer, INET_ADDRSTRLEN);
				}
			}
			else
			{
				if (!strcmp(device, ifa->ifa_name))
				{
					mySockAddress.sin_addr = ((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
					printf("Address saved\n");
					memcpy(address, addressBuffer, INET_ADDRSTRLEN);
				}
			}

		}
		else if (ifa->ifa_addr->sa_family == AF_INET6)
		{ // check it is IP6
			// is a valid IP6 Address
			tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
			char addressBuffer[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
			printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer);
		}
	}
	if (ifAddrStruct!=NULL)
	{
		freeifaddrs(ifAddrStruct);
	}

	bind(this->socketFileDescriptor, (struct sockaddr*)&mySockAddress, sizeof(sockaddr_in));

//	getsockname(this->socketFileDescriptor, (struct sockaddr*)&mySockAddress, &socklen);
	socklen = 16;
	printf("my IP Address: %x\n", mySockAddress.sin_addr.s_addr);
	this->myAddress = new SocketAddressNetwork(mySockAddress, socklen, this->uniqueID);
}

SocketIO_Network::SocketIO_Network(sockaddr_in* address)
{
	printf("Creating SocketIO_Network(%d)\n", address->sin_addr.s_addr);
	char uid[25];
	snprintf(uid, 25, "UID=%ld", (long) getpid());
	this->uniqueID = uid;

	myAddress = new SocketAddressNetwork(*address, sizeof(*address), this->uniqueID);

	this->socketFileDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
	bind(this->socketFileDescriptor, (struct sockaddr*)address, sizeof(sockaddr_in));
}

SocketIO_Network::~SocketIO_Network()
{
	close(this->socketFileDescriptor);
	delete this->myAddress;
}

std::string SocketIO_Network::getUniqueID()
{
    return this->uniqueID;
}

struct SocketAddress* SocketIO_Network::getAddress()
{
    return ((SocketAddress*) this->myAddress);
}

int SocketIO_Network::getSocketFileDescriptor()
{
	return this->socketFileDescriptor;
}

int SocketIO_Network::send(void* data, int numOfBytes)
{
    return 0;
}

int SocketIO_Network::receive(void* data, int numOfBytes)
{
    return recvfrom(this->socketFileDescriptor, data, numOfBytes, 0, NULL, NULL);
}

