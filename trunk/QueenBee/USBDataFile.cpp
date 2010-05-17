/*
 * USBDataFile.cpp
 *
 *  Created on: 2010-4-29
 *      Author: mxx
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "USBDataFile.h"
#include "DebugLog.h"

USBDataFile::USBDataFile() {
	pData = new USBFile;
}

USBDataFile::~USBDataFile() {
	if (pData) delete pData;
}

void USBDataFile::Init(void)
{
	memset(pData,0xFF,sizeof(USBFile));
	pData->para.mark = 0x30aa;
	pData->para.IBBType = 0x3000;

	DEBUG("size of StructPT = %d",sizeof(StructPT));
	PartitionTable& Table = pData->table;
	Table.Available = PartitionTableFlag;
	Table.DoubtPointData.BaseAddr =  DPD_BASE;
	Table.DoubtPointData.EndAddr =   DPD_END;
	Table.DoubtPointData.CurPoint =  DPD_BASE;

	Table.RunRecord360h.BaseAddr = RR360H_BASE;
	Table.RunRecord360h.EndAddr =  RR360H_END;
	Table.RunRecord360h.CurPoint = RR360H_BASE;

	Table.BaseData.BaseAddr = BASEDATA_BASE;
	Table.BaseData.EndAddr = BASEDATA_END;
	Table.BaseData.CurPoint = BASEDATA_BASE;

	Table.TotalDistance = 0;
	Table.DriverCode = 10000000;//驾驶员代码
	for( int i = 0; i < 20; i++)
		Table.DriverLisenseCode[i] = 0;//驾驶证号码
	Table.InOSAlarmCycle = 0;//“在分路段报警周期中”标志
	Table.OSAlarmAddupDistance = 0;//分路段报警路程累计

}
void USBDataFile::InitPara(StructPara& para)
{
    pData->para = para;
}
void USBDataFile::Save(const char* szPath)
{
    FILE* f = fopen(szPath,"wb");
    fwrite((const char*)pData,1,sizeof(USBFile),f);
    fclose(f);
}
