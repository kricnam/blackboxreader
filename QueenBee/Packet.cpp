/*
 * Packet.cpp
 *
 *  Created on: 2010-4-14
 *      Author: mxx
 */

#include "Packet.h"
#include "DebugLog.h"
#include <stdio.h>
#include "USBDataFile.h"

Packet::Packet()
{
	// TODO Auto-generated constructor stub

}

Packet::~Packet()
{
	// TODO Auto-generated destructor stub
}

void Packet::SetCmdPacket(CmdWord cmd)
{
	data.clear();
	data=0xAA;
	data+=0x75;
	data+=cmd;
	data.append(3,0);
	data+=XOR();
}

int Packet::GetDriverCode(void)
{
	struct PacketHead *Head = (struct PacketHead *)GetData();
	if (Head->cCmdWord == GET_DriverID_LicenceID)
	{
		struct DriverInfo* info = (struct DriverInfo*)(Head+1);
		return ((info->DriverCode[0]<<16)&0xFF0000)|
				((info->DriverCode[1]<<8)&0xFF00)|
				info->DriverCode[2];
	}
	return -1;
}

string Packet::GetLicenseID(void)
{
	struct PacketHead *Head = (struct PacketHead *)GetData();

	if (Head->cCmdWord == GET_DriverID_LicenceID)
	{
		struct DriverInfo* info = (struct DriverInfo*)(Head+1);
		string id(info->LicenceNumber,18);
		return id;
	}
	return "";
}

int Packet::GetAllPara(USBDataFile::StructPara& para)
{
  struct PacketHead *Head = (struct PacketHead *)GetData();

  if (Head->cCmdWord == GET_ALL_PARA)
  {
      USBDataFile::StructPara* pPara = (USBDataFile::StructPara*)(Head+1);
      memcpy(&para,pPara,256);
      return 1;
  }
  return 0;
}

struct Packet::AccidentData* Packet::GetAccidentData(int& num)
{
	struct PacketHead *Head = (struct PacketHead *)GetData();
	num = 0;
	if (Head->cCmdWord == GET_Accident_Data)
	{
		int n = ((Head->Len[0]<< 8)&0xFF00) | (Head->Len[1]);
		num = n / sizeof(struct AccidentData);
		return (struct AccidentData*)(Head+1);
	}
	return NULL;
}

struct Packet::SpeedRecord* Packet::GetSpeedData(int& num)
{
        struct PacketHead *Head = (struct PacketHead *)GetData();
        num = 0;
        if (Head->cCmdWord == GET_360Hour_Speed || Head->cCmdWord == GET_2Day_Speed)
        {
                int n = ((Head->Len[0]<< 8)&0xFF00) | (Head->Len[1]);
                num = n - sizeof(struct SpeedRecord) + 1;
                return (struct SpeedRecord*)(Head+1);
        }
        return NULL;
}

unsigned char Packet::XOR()
{
	string::iterator it;
	unsigned char sum=0;
	it = data.begin();
	sum = *it;
	it++;
    for ( ; it < data.end(); it++ )
	    sum ^= *it;
    return sum;
}

void Packet::ReceiveFrameFrom(RS232Port & port)
{
	char buff;
	int n;
	int count = 0;
	int err=0;

	data.clear();
	frameState = NONE;
	do
	{
		n = port.Read(&buff, 1);
		if (n == 0)
		{
			data.clear();
			return;
		}
		TRACE("%02hhX ",buff);

		switch (frameState)
		{
		case NONE:
			if (buff == 0x55) frameState = SOF1;
			else {err++; frameState = NONE;	data.clear();}
			break;
		case SOF1:
			if (buff == 0x7A) frameState = SOF2;
			else { err++; frameState = NONE; data.clear();}
			break;
		case SOF2:
			frameState = CMD;
			break;
		case CMD:
			if (buff == GET_CMD_ERROR || buff == SET_CMD_ERROR)
			{
				frameState = DUMMY;
			}
			else
			{
				frameState = SIZE1;
				count = ((buff&0xFF) << 8);
			}
			break;
		case SIZE1:
			frameState = SIZE2;
			count = (count&0xFF00)|(buff&0xFF);
			break;
		case SIZE2:
			frameState = DUMMY;
			break;
		case DUMMY:
			if (!count) frameState = CHECKSUM;
			else frameState = DATA;
			break;
		case DATA:
			count--;
			frameState = DATA;
			if (!(count-1)) frameState=CHECKSUM;
			break;
		case CHECKSUM:
			TRACE("FRAME[CHECKSUM]");
			if (buff == XOR())
			{
				data.append(&buff,1);
			}
			else
			{
				DEBUG("CHECK SUM ERROR");
				data.clear();
			}
			return;
		};
		TRACE("FRAME[%d]",frameState);
		if (frameState!=NONE) data.append(&buff,1);
	} while (err < 0xFFFF);
}

void Packet::Dump()
{
	char buf[1024]={0};
	int n=0;
	for(unsigned int i=0;i < data.size();i++)
	{
		sprintf(buf+n,"%02hhX ",data[i]);
		n = strlen(buf);
		if (n>1000) break;
	}
	DEBUG(buf);
}

