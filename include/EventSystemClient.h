#ifndef EVENTSYSTEMPARTICIPANTIMPL_H
#define EVENTSYSTEMPARTICIPANTIMPL_H

#define DEBUG_OUT

#include "Telegram/Telegram.h"
#include <string>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "EventSystemParticipant.h"
#include "SocketSlave.h"

namespace EventSystem
{

/**
 * this class contains a socket either local or network, and a receive thread
 */
class EventSystemClient : public EventSystemParticipant
{
    public:
		/**
		 * constructs a local client
		 * @param id: the identification string used to address the client
		 */
        EventSystemClient(std::string id);

        /**
         * constructs a network client
         * @param id: the identification string used to address the client
         * @param port: should be the default port, @see constants.h
         * @networkDevice: @see SocketSlave.h
         */
        EventSystemClient(std::string id, in_port_t port, char* networkDevice);
        virtual ~EventSystemClient();

        /**
         * called if the client is initialized to work locally, sends telegram to master
         */
        int connectToMaster();

        /**
         * called if the client is initialized to work via network, sends telegram to master
         * @param address: the address of the master
         */
        int connectToMaster(sockaddr_in address);

        /**
         * starts the receiving thread
         * must be called to receive anything!
         */
        void startReceiving();

        /**
         * stops the receiving thread
         * also unregisters from master
         * call startReceiving() & connectToMaster() to revert the process
         */
        void stopReceiving();

        std::string getIdentifier();
        std::string getUniqueIdentifier();

        SocketAddress* getAddress();
        void setAddress(sockaddr_in address, socklen_t len);

        virtual SocketIO* getSocket();
        void* getMessageMemory();

        void send(Telegram* telegram);
        void send(void* data, int numOfBytes);
        virtual void log(Telegram_Object* log);

        //TODO: request routine (answers telegram with uniqueID as source id)
        //TODO: set function pointer for own request routine

        /**
         * receive data
         * @param data: received data written to
         * @param nonblocking: specify if this returns immediately or block till data has arrived
         * @return 0 if no data was available, -1 on error, number of received bytes on success
         */
        int receive(void* data, bool nonblocking);

        void setMessageReceived(bool newMessage);
        void setReceivedBytes(unsigned int numOfBytes);
        unsigned int getReceivedBytes();

        pthread_mutex_t* getMemoryMutex();
        pthread_cond_t* getReceivedCondition();
        pthread_cond_t* getFetchedCondition();
    protected:
    private:
        void init(std::string id);
        void disconnect();

        unsigned int receivedBytes;

        pthread_mutex_t memoryMutex;
        pthread_cond_t messageReceived;
        pthread_cond_t messageFetched;

        std::string id;
        Socket_Slave socket;
        void* messageMemory;    //Mutex!
        void* sendMemory;
        pthread_t connectThreadID;
        bool newMessage;        //Mutex!
};

} /* namespace EventSystem */
#endif // EVENTSYSTEMPARTICIPANTIMPL_H
