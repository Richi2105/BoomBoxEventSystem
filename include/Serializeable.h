/*
 * Serializeable.h
 *
 *  Created on: Nov 28, 2015
 *      Author: richard
 */

#ifndef INCLUDE_SERIALIZEABLE_H_
#define INCLUDE_SERIALIZEABLE_H_

#include <stdint.h>
#include <string.h>
#include <OS_DEF.h>

#define packData(dest, source) 	memcpy(dest, &source, sizeof(source)); \
								dest += sizeof(source);

#define packNData(dest, source, n) 	memcpy(dest, source, sizeof(source[0])*n); \
									dest += sizeof(source[0])*n;

#define unpackData(source, dest)	memcpy(&dest, source, sizeof(dest)); \
									source += sizeof(dest);

#define unpackNData(source, dest, n)	memcpy(dest, source, sizeof(dest[0])*n); \
										source += sizeof(dest[0])*n;


class Serializeable
{
public:
	virtual int getSerializedSize() = 0;
	virtual int serialize(void* const data) = 0;
	virtual int deserialize(void const * const data) = 0;
	virtual ~Serializeable() {};
};



#endif /* INCLUDE_SERIALIZEABLE_H_ */
