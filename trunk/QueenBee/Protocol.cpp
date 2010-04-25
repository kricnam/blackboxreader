/*
 * Protocol.cpp
 *
 *  Created on: 2010-4-16
 *      Author: mxx
 */

#include "Protocol.h"

Protocol::Protocol()
{
	// TODO Auto-generated constructor stub

}

Protocol::~Protocol()
{
	// TODO Auto-generated destructor stub
}


int Protocol::GetDriverIDLicenceID(RS232Port &port)
{
	Packet packet;
	packet.SetCmdPacket(Packet::GET_DriverID_LicenceID);
	int n = packet.GetSize();
	const char* c=packet.GetData();
	//for(int i=0;i<n;i++)
	//{
	//	port.Write(c+i,1);
		//usleep(200);
	//}
	if (n == port.Write(packet.GetData(),n))
	{
		sleep(1);
		packet.ReceiveFrameFrom(port);
		packet.Dump();
	}
	return 0;
}










