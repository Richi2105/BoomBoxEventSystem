#ifndef EVENTSYSTEMMASTER_H
#define EVENTSYSTEMMASTER_H

#define DEBUG_OUT

#include <string>
#include <sys/un.h>

#include "EventSystemParticipant.h"
#include "SocketAddress.h"
#include "../include/Telegram/Telegram.h"

#include <vector>
#include <map>
#include <thread>

#include "SocketMaster.h"

namespace EventSystem
{

typedef std::vector<SocketAddressLocal> LocalClientVector;
typedef std::vector<SocketAddressNetwork> NetworkClientVector;
typedef std::map<std::string, LocalClientVector> LocalClientMap;
typedef std::map<std::string, NetworkClientVector> NetworkClientMap;


typedef void* (threadfunction)(void* arg);

/**
 * this class holds two sockets, local and network
 * has 2 threads for receiving messages, directs messages to connected clients
 */
class EventSystemMaster : public EventSystemParticipant
{
public:
	/**
	 * construct a default master
	 */
	EventSystemMaster();

	/**
	 * specify a network device, like eth0 or wlan0
	 */
	EventSystemMaster(char* networkDevice);
	virtual ~EventSystemMaster();

	virtual std::string getIdentifier();
	virtual std::string getUniqueIdentifier();

	virtual SocketIO* getSocket();
	SocketIO_Local* getLocalSocket();
	SocketIO_Network* getNetworkSocket();
	virtual SocketAddress* getAddress();

	void addClient(std::string id, SocketAddressLocal* address);
	void addClient(std::string id, SocketAddressNetwork* address);

	void removeClient(std::string id, SocketAddressLocal* remAddress);
	void removeClient(std::string id, SocketAddressNetwork* remAddress);

	void sendToClient(std::string destination, bool isUniqueID, void* data, int numOfBytes);
	virtual void log(Telegram_Object* log);

	void setLoggerConnected();
	bool isLoggerConnected();

protected:
private:
	void init();

	std::thread* threadCheckMessageLocal;
	std::thread* threadCheckMessageNetwork;

	std::string id;
	std::string uniqueID;

	Socket_Master master;

	void* dataPointer;

	LocalClientMap localClients;
	NetworkClientMap networkClients;
	std::vector<std::string> clientRoles;

	bool loggerConnected;
};

} /* namespace EventSystem */

#endif // EVENTSYSTEMMASTER_H
