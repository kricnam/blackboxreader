/*
 * Protocol.cpp
 *
 *  Created on: 2010-4-16
 *      Author: mxx
 */

#include "Protocol.h"
#include "stdio.h"

Protocol::Protocol()
{
	// TODO Auto-generated constructor stub

}

Protocol::~Protocol()
{
	// TODO Auto-generated destructor stub
}

int Protocol::Read(Packet::CmdWord cmd,int wait_ms,RS232Port & port,Packet& packet)
{
	packet.SetCmdPacket(cmd);
	int n = packet.GetSize();
	const char* c=packet.GetData();
	if (n == port.Write(c,n))
	{
		usleep(wait_ms*1000);
		packet.ReceiveFrameFrom(port);
		packet.Dump();
		return packet.GetSize();
	}
	return -1;
}

int Protocol::ReadDriverIDLicenceID(RS232Port &port,Packet& packet)
{
	Read(Packet::GET_DriverID_LicenceID,1000,port,packet);
	printf("code:%d\nlicense:%s\n",packet.GetDriverCode(),packet.GetLicenseID().c_str());
	return 0;
}

int Protocol::ReadRealTime(RS232Port & port,Packet& packet)
{
	return Read(Packet::GET_RealTime,1000,port,packet);
}

int Protocol::Read360HourMileage(RS232Port & port,Packet& packet)
{
	return Read(Packet::GET_360Hour_Mileage,10000,port,packet);
}
int Protocol::ReadVehicleFeature(RS232Port & port,Packet& packet)
{
	return Read(Packet::GET_Vehicle_Feature,1000,port,packet);
}
int Protocol::Read360HourSpeed(RS232Port & port,Packet& packet)
{
	return Read(Packet::GET_360Hour_Speed,10000,port,packet);
}
int Protocol::ReadVehicleVIN_Number_Class(RS232Port & port,Packet& packet)
{
	return Read(Packet::GET_Vehicle_VIN_Number_Class,1000,port,packet);
}
int Protocol::ReadAccident_Data(RS232Port & port,Packet& packet)
{
	int n=Read(Packet::GET_Accident_Data,2000,port,packet);
	if (n>0)
	{
		int n;
		struct Packet::AccidentData *Data = packet.GetAccidentData(n);
//		if (Data)
//		{
//			printf("%d%d-%d%d-%d%d %d%d:%d%d:%d%d %d %02hhX\n",
//					(Data[0].bcdYear&0xF0)>>4,
//					(Data[0].bcdYear&0x0F),
//					(Data[0].bcdMonth&0xF0)>>4,
//					(Data[0].bcdMonth&0x0F),
//					(Data[0].bcdDay&0xF0)>>4,
//					(Data[0].bcdDay&0x0F),
//					(Data[0].bcdHour&0xF0)>>4,
//					(Data[0].bcdHour&0xF),
//					(Data[0].bcdMinute&0xF0)>>4,
//					(Data[0].bcdMinute&0xF),
//					(Data[0].bcdSecond&0xf0)>>4,
//					(Data[0].bcdSecond&0xf),
//					Data[0].Speed,
//					Data[0].SwitchState
//					);
//		}
	}
	return 0;
}
int Protocol::Read2DayMileage(RS232Port & port,Packet& packet)
{
	return Read(Packet::GET_2Day_Mileage,1000,port,packet);
}
int Protocol::Read2DaySpeed(RS232Port & port,Packet& packet)
{
	return Read(Packet::GET_2Day_Speed,2000,port,packet);
}
int Protocol::Read2DayOverDrive(RS232Port & port,Packet& packet)
{
	return Read(Packet::GET_2Day_OverDrive,10000,port,packet);
}

int Protocol::ReadAllPara(RS232Port & port,Packet& packet)
{
    return Read(Packet::GET_ALL_PARA,2000,port,packet);
}





