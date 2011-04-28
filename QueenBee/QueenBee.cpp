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

void initLED(void)
{
	system("echo 45 > /sys/class/gpio/export");
	system("echo 46 > /sys/class/gpio/export");
	system("echo high > /sys/class/gpio/gpio45/direction");
	system("echo high > /sys/class/gpio/gpio46/direction");
}

void turnOnLEDGreen(void)
{
	system("echo low > /sys/class/gpio/gpio45/direction");
}

void turnOnLEDRed(void)
{
	system("echo low > /sys/class/gpio/gpio46/direction");
}

void turnOffLEDGreen(void)
{
	system("echo high > /sys/class/gpio/gpio45/direction");
}

void turnOffLEDRed(void)
{
	system("echo high > /sys/class/gpio/gpio46/direction");
}

void blinkRed(void)
{
	for(int i = 0;i<3;i++)
	{
	turnOffLEDRed();
	sleep(1);
	turnOnLEDRed();
	sleep(1);
	}
	turnOffLEDRed();
}

int main(int argc, char** argv)
{
	initLED();
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
  protocol.Reset(port);

  sleep(5);

  turnOnLEDRed();
  protocol.ReadAllPara(port, packet);
  if (!packet.GetAllPara(para))
    {
      INFO("Can not read data from remote");
      blinkRed();
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
  turnOffLEDRed();
  sleep(1);
  int retry = 10;
  INFO("read data");

  turnOnLEDRed();
  do {
    protocol.Read2DaySpeed(port,packet);
    if (file.AddSpeedData(packet)) break;
  } while(--retry);
  turnOffLEDRed();

  if (!retry) goto onerror;
  sleep(1);
  turnOnLEDRed();
  retry=10;
  INFO("read accident data");
  do
    {
      protocol.ReadAccident_Data(port, packet);
      if (file.AddAccidentData(packet))
        break;
      sleep(1);
    }
  while (--retry);
  turnOffLEDRed();

  if (!retry) goto onerror;
  protocol.Over(port);
  file.Save(strName.c_str());
  turnOnLEDGreen();
  return 0;

onerror:
INFO("Can not read data from remote");
  blinkRed();
  return -1;
}
