/*
 * QueenBee.cpp
 *
 *  Created on: 2010-4-14
 *      Author: mxx
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DebugLog.h"
#include "Protocol.h"
#include "RS232Port.h"
#include "Packet.h"
#include "USBDataFile.h"

int main(int argc, char** argv)
{
  if (argc > 2)  SETTRACELEVEL(atoi(argv[2]));
  else SETTRACELEVEL(2);
  RS232Port port;
  Packet packet;
  Protocol protocol;
  port.Open(argv[1]);

  //protocol.ReadDriverIDLicenceID(port,packet);
  //protocol.ReadRealTime(port,packet);
  //protocol.Read2DayMileage(port,packet);

  //protocol.ReadAccident_Data(port,packet);



  USBDataFile file;
  USBDataFile::StructPara para;
  struct Packet::SpeedRecord* pSpeed;
  int nRec;

  memset(&para,0,sizeof(para));
//
//  para.mark = 0x30aa;
//  para.IBBType = 0x3000;
//  strcpy((char*)para.sn,"1234567890");
//  strcpy((char*)para.DriverLisenseCode,"abcdefghijklmnopqrst");
//  para.DriverCode = 10000000;
//  para.CHCO = 0x12345678;
//  strcpy((char*)para.AutoVIN,"V1234567890");
//  strcpy((char*)para.AutoCode,"WJ12345678");
//

  file.Init();
  INFO("read para part");
  protocol.ReadAllPara(port,packet);
  packet.GetAllPara(para);
  file.InitPara(para);

  INFO("read speed");
  protocol.Read360HourSpeed(port,packet);
  file.AddSpeedData(packet);


  //protocol.Read360HourMileage(port,packet);
  //protocol.ReadAccident_Data(port,packet);
  //protocol.Read2DayOverDrive(port,packet);
  file.Save("TEST.IBB");

	return 0;
}
