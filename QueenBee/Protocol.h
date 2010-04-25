/*
 * Protocol.h
 *
 *  Created on: 2010-4-16
 *      Author: mxx
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_
#include "Packet.h"
#include "RS232Port.h"
class Protocol
{
public:
	Protocol();
	virtual ~Protocol();
	int GetDriverIDLicenceID(RS232Port & port);

};

#endif /* PROTOCOL_H_ */
