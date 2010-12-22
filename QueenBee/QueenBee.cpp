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

  USBDataFile file;
  USBDataFile::StructPara para;
  string strName;

  memset(&para,0,sizeof(para));

  INFO("read para part");
  protocol.ReadAllPara(port,packet);
  if (!packet.GetAllPara(para))
   {
    INFO("Can not read data from remote");
    return -1;
    }

   char szName[sizeof(para.AutoCode)+1]={0};

   strncpy(szName,(const char*)para.AutoCode,sizeof(para.AutoCode));
   for(unsigned int i=0;i<sizeof(szName);i++)
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

  INFO("read data");
  protocol.Read2DaySpeed(port,packet);
  file.AddSpeedData(packet);
  INFO("read accident data");
  protocol.ReadAccident_Data(port,packet);
  file.AddAccidentData(packet);

  file.Save(strName.c_str());
  return 0;
}
