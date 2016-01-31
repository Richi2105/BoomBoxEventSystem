#ifndef EVENTSYSTEMPARTICIPANTIMPL_H
#define EVENTSYSTEMPARTICIPANTIMPL_H

#include "Telegram/Telegram.h"
#include <string>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "StateMachine/StateMachine.h"
#include "Watchdog/Watchdog.h"
#include "EventSystemParticipant.h"
#include "SocketSlave.h"

namespace EventSystem
{

/**
 * Class for sending / Receiving Telegrams over the EventSystem in a Linux / Unix environment.
 * First, construct a new EventSystemClient object with a name (works locally on the same machine)
 * or construct with a name, port number (given in constants.h file) and a device identifier (something like eth0 or wlan0)
 *
 * call connectToMaster() for local operation or connectToMaster(address) with the address of the machine running the master.
 *
 * If something should be received, call startReceiving(). It starts a listen thread. Received data can be fetched via receive()
 *
 * send() sends a telegram to a destination specified in the telegram header
 *
 * this class contains a socket either local or network, and a receive thread
 * a state machine can be accessed via ->statemachine, used for documenting the state of the program.
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
         * @param softStop waits till master gets status, then stops
         */
        void stopReceiving(bool softStop);

        std::string getIdentifier();
        std::string getUniqueIdentifier();

        SocketAddress* getAddress();
        void setAddress(sockaddr_in address, socklen_t len);

        virtual SocketIO* getSocket();

        Socket_Slave* getSocketSlave();

        void* getMessageMemory();

        void send(Telegram* telegram);
        void send(void* data, int numOfBytes);
        virtual void log(Telegram_Object* log);

        //TODO: request routine (answers telegram with uniqueID as source id)
        //TODO: set function pointer for own request routine

        /**
         * receive data, fetches inbox
         * automatically sets statemachine to RECEIVING, and afterwards to IDLE.
         * @param data: received data written to
         * @param waitTime: specify if this returns immediately (-1), or return after waitTime seconds (0 to wait till data arrives)
         * @return 0 if no data was available, -1 on error, number of received bytes on success
         */
        int receive(void* data, int waitTime);

        /**
         * if a message is in inbox, it is deleted, 0 is returned
         * if no message is in inbox, -1 is returned.
         */
        int discardMessage();

        /**
         * @return: if the client is connected to the master, i.e. you can receive messages
         */
        bool isConnected();

        /**
         * if this returns true, the master might has been stopped (crashed)
         * @return: if there has been an error on the connection, i.e. you can't send messages
         */
        bool isConnectionError();

        void setMessageReceived(bool newMessage);
        void setReceivedBytes(unsigned int numOfBytes);
        unsigned int getReceivedBytes();

        pthread_mutex_t* getMemoryMutex();
        pthread_cond_t* getReceivedCondition();
        pthread_cond_t* getFetchedCondition();

        /**
         * object that can be accessed by the surrounding program to set it's state.
         * it is read by the master and the state is logged
         */
        StateMachine* statemachine;

        /**
         * object to watch the connection to the master. if a status request was not received,
         * after a certain time it will alarm the client and the connection will be closed.
         */
        Watchdog* watchdog;

        /**
         * to be called by the watchdog thread
         */
        void setConnectionError();

    protected:
    private:
        void disconnect();

        unsigned int receivedBytes;

        pthread_mutex_t memoryMutex;
        pthread_cond_t messageReceived;
        pthread_cond_t messageFetched;

        std::string id;
        Socket_Slave socket;
        void* messageMemory;    //Mutex!
        void* sendMemory;
        pthread_t messageThreadID;	/** <controls the inbox of the client. */
        pthread_t watchdogThreadID;	/** <controls the watchdog of the client. */
        bool newMessage;        //Mutex!

        bool run_MessageThread;
        bool run_WatchdogThread;

        bool isStopped;
        bool connected;
        bool connectionError;
};

} /* namespace EventSystem */
#endif // EVENTSYSTEMPARTICIPANTIMPL_H
