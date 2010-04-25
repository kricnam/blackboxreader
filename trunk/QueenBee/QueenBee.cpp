/*
 * QueenBee.cpp
 *
 *  Created on: 2010-4-14
 *      Author: mxx
 */
#include <unistd.h>
#include <stdio.h>

#include "Protocol.h"
#include "RS232Port.h"
#include "Packet.h"
int main(int argc, char** argv)
{
	RS232Port port;
	Packet packet;
	Protocol protocol;
	port.Open(argv[1]);
	protocol.GetDriverIDLicenceID(port);
/*
	char a[250];

	int n;
	do
	{
		n= port.Read(a,250);

		if (!n) sleep(2);
		else
		{
			for(int i=0;i<n;i++)
				printf("%02hhX ",a[i]);
			printf("\n");
			char b[]={0x55,0x7A, 0x01 ,0x00,0x15 ,0x00,0x00, 0x22,0xB8 ,0x36,0x36, 0x36 ,0x36 ,00 ,00,0xA8, 0x94, 0xE5 ,0x77, 0x3C ,0xF1 ,0x12 ,00, 0xD0, 0xF2, 0x12, 00,0xE0};
			port.Write(b,sizeof(b));
		}
	}while(1);

*/
	return 0;
}
