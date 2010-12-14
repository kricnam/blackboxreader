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
#include <string>
#include "DebugLog.h"
#include "Protocol.h"
#include "RS232Port.h"
#include "Packet.h"
#include "USBDataFile.h"
using namespace std;
int main(int argc, char** argv)
{
  RS232Port port;
  Packet packet;
  Protocol protocol;
  port.Open(argv[1]);

  if (argc > 3)  SETTRACELEVEL(atoi(argv[3]));
  else SETTRACELEVEL(3);
  if (argc > 2)
    protocol.SetOTA(atoi(argv[2])>0);

  //protocol.ReadDriverIDLicenceID(port,packet);
  //protocol.ReadRealTime(port,packet);
  //protocol.Read2DayMileage(port,packet);

  //protocol.ReadAccident_Data(port,packet);



  USBDataFile file;
  USBDataFile::StructPara para;
  struct SpeedRecord* pSpeed;
  int nRec;
  string strName;

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


  INFO("read para part");
  protocol.ReadAllPara(port,packet);
  if (!packet.GetAllPara(para))
    {
    INFO("Can not read data from remote");
    return -1;
    }

   char szName[sizeof(para.AutoCode)+1]={0};

   strncpy(szName,(const char*)para.AutoCode,sizeof(para.AutoCode));
   for(int i=0;i<sizeof(szName);i++)
    {
       if (szName[i]==0xFF)
         {
           szName[i]=0;
         }
    }
    strName.append(szName);
    strName+=".IBB";
    DEBUG("set file name as %s",strName.c_str());

  file.InitPara(para);
  file.Init();

  INFO("read speed");
  protocol.Read360HourSpeed(port,packet);
  file.AddSpeedData(packet);


  //protocol.Read360HourMileage(port,packet);
  //protocol.ReadAccident_Data(port,packet);
  //protocol.Read2DayOverDrive(port,packet);

  file.Save(strName.c_str());
  return 0;
}
