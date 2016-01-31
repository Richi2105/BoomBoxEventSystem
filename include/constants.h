/*
 * constants.h
 *
 *  Created on: Dec 22, 2015
 *      Author: richard
 */

#ifndef INCLUDE_CONSTANTS_H_
#define INCLUDE_CONSTANTS_H_

/**
 * the id length of an EventSystemParticipant
 */
#define ID_SIZE 20

/**
 * the length of the unique id
 */
#define UNIQUEID_SIZE 30

/**
 * the length of a function description (used by TokenDaemon)
 */
#define FUNCTIONDESCRIPTION_SIZE 30

/**
 * max characters in a log message
 */
#define LOG_MESSAGE_SIZE 250

/**
 * max characters Key Description
 */
#define KEYDESCRIPTION_SIZE 20

/**
 * maximum telegram size, default data allocation size
 */
#define DATASIZE 4096

/**
 * maximum chararcters of a custom state
 */
#define CUSTOMSTATE 20

/**
 * the standard port for network communication
 * 26901: Eve Online
 * 27000: QuakeWorkld
 */
#define PORT 26999




#endif /* INCLUDE_CONSTANTS_H_ */
