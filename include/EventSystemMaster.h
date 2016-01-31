#ifndef EVENTSYSTEMMASTER_H
#define EVENTSYSTEMMASTER_H

#include <string>
#include <sys/un.h>

#include "EventSystemParticipant.h"
#include "SocketAddress.h"
#include "Telegram/Telegram.h"
#include "Watchdog/ClientWatchdog.h"
#include "StateMachine/StateObject.h"

#include <vector>
#include <map>
#include <thread>

#include "SocketMaster.h"

namespace EventSystem
{

/**
 * stores references to all clients either network-y or locally inside a ClientWatchdog-Object
 * for easier accessing all clients
 */
typedef std::vector<ClientWatchdog*> ClientVector;

typedef std::vector<SocketAddressLocal*> LocalClientVector;
typedef std::vector<SocketAddressNetwork*> NetworkClientVector;

typedef std::map<std::string, ClientVector*> ClientMap;
typedef std::pair<std::string, ClientVector*> ClientPair;

/**
 * stores the clients based on their connection:
 * local clients
 */
typedef std::map<std::string, LocalClientVector> LocalClientMap;
/**
 * stores the clients based on their connection:
 * network clients
 */
typedef std::map<std::string, NetworkClientVector> NetworkClientMap;

typedef std::pair<std::string, LocalClientVector> LocalClientPair;
typedef std::pair<std::string, NetworkClientVector> NetworkClientPair;

typedef void* (threadfunction)(void* arg);


/**
 * EventSystemMaster handles Telegrams sent from a client and puts them trough to the client
 * specified in the header.
 * There are two modes:
 * 		-an id is specified in the header, the master tries to send the
 * 		 telegram to all clients matching the id
 * 		-the isUnique byte in the header is set. the master sends it to the only client matching
 * 		 the uniqueID.
 *
 * Clients have to register via a Register_Local or Register_Network object to receive messages.
 *
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

	//clientMutex lock
	void addClient(std::string id, SocketAddress* address);

	//clientMutex lock
	void removeClient(std::string id, SocketAddress* address);

	//clientMutex lock
	inline ClientWatchdog* getClientByUID(std::string uid);

	//clientMutex lock
	void sendToClient(std::string destination, bool isUniqueID, void* data, int numOfBytes);

	//clientMutex lock
	virtual void log(Telegram_Object* log);

	void inkrementConnectedLogger();
	void dekrementConnectedLogger();

	ClientMap* clients;

    /**
     * only one mutex for both containers
     */
    pthread_mutex_t clientMutex;
    pthread_mutex_t pingMutex;
    pthread_cond_t pingCondition;

    char* currUIDPingResponse;
    StateObject* currStatusPingResponse;

    Socket_Master master;
protected:
private:
	void init();

    pthread_t local_messageThreadID;
    pthread_t network_messageThreadID;

    pthread_t checkStatusID;
    pthread_t getStatusID;

    bool run_localMessageThread;
    bool run_networkMessageThread;
    bool run_checkStatusThread;
    bool run_getStatusThread;

	std::string id;
	std::string uniqueID;

	void* dataPointer;

	int nLoggerConnected;
};

} /* namespace EventSystem */

#endif // EVENTSYSTEMMASTER_H
