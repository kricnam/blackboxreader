/*
 * USBDataFile.cpp
 *
 *  Created on: 2010-4-29
 *      Author: mxx
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "USBDataFile.h"
#include "DebugLog.h"
#include "Packet.h"
#define BCD_CHAR(x)    ((((x)>>4)&0x0F)*10+((x)&0x0F))
#define CHAR_BCD(x)     (((((x)&0xFF)/10)<<4)|(((x)&0xFF)%10))
USBDataFile::USBDataFile() {
	pData = new USBFile;
	if (pData)
	  memset(pData,0xFF,sizeof(USBFile));
}

USBDataFile::~USBDataFile() {
	if (pData) delete pData;
}

void USBDataFile::Init(void)
{
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
	Table.DriverCode = pData->para.DriverCode;//驾驶员代码
	memcpy(Table.DriverLisenseCode,pData->para.DriverLisenseCode,20);//驾驶证号码
	Table.InOSAlarmCycle = 0;//“在分路段报警周期中”标志
	Table.OSAlarmAddupDistance = 0;//分路段报警路程累计
}

void USBDataFile::InitPara(StructPara& para)
{
    pData->para = para;
}

int USBDataFile::AddSpeedData(Packet &p)
{
  int n=0;
  if (p.GetSize()) return 0;
  Packet::SpeedRecord* pRec = p.GetSpeedData(n);
  if (!pRec) return 0;
  char* data = (char*)&(pRec->speed);
  char* cache = (char*)pData;

  DEBUG("Total %d minutes data",n);
  int nStart = pData->table.BaseData.BaseAddr;
  int nEnd = pData->table.BaseData.EndAddr;
  int nCur = nStart;
  DEBUG("start address:%d",nStart);
  RecordData_start* rec_start = (RecordData_start*)(cache+nStart);
  RecordData_end* rec_end;

  rec_start->dt.type = 0xFEFE;
  rec_start->dt.year = pRec->startTime.bcdYear;
  rec_start->dt.month = pRec->startTime.bcdMonth;
  rec_start->dt.day =  pRec->startTime.bcdDay;
  rec_start->dt.hour = pRec->startTime.bcdHour;
  rec_start->dt.minute = pRec->startTime.bcdMinute;
  rec_start->dt.second = 0;

  DEBUG("start at %d-%d-%d %d:%d",BCD_CHAR(pRec->startTime.bcdYear),
      BCD_CHAR(pRec->startTime.bcdMonth),
      BCD_CHAR(pRec->startTime.bcdDay),
      BCD_CHAR(pRec->startTime.bcdHour),
      BCD_CHAR(pRec->startTime.bcdMinute));

  for (int i = 0; i < n; i++)
    {
      for (int ii = 0; ii < 60; ii++)
        {
          if ((nCur + sizeof(rec_end))>= nEnd)
            {
              INFO("Data exceed at %d:%d",i,ii);
              n=i;
              break;
            }
          cache[nStart + sizeof(RecordData_start) + i * 120 + 2*ii] = data[i];
          cache[nStart + sizeof(RecordData_start) + i * 120 + 2*ii + 1] = 0;
          nCur = nStart + sizeof(RecordData_start) + i * 120 + 2*ii + 2;
        }
    }

  DEBUG("data fill ok");
  rec_end = (RecordData_end*)(cache+ nCur);

  memcpy(rec_end,rec_start,sizeof(RecordData_start));
  incTime(rec_end->dt,n);
  rec_end->dt.type = 0xAFAF;
  DEBUG("end at %d-%d-%d %d:%d",BCD_CHAR(rec_end->dt.year),
       BCD_CHAR(rec_end->dt.month),
       BCD_CHAR(rec_end->dt.day),
       BCD_CHAR(rec_end->dt.hour),
       BCD_CHAR(rec_end->dt.minute));
  return 0;
}

void USBDataFile::incTime(Record_CLOCK& t, int nMinute)
{
    time_t tv;
    struct tm tmTime={0};
    tmTime.tm_year = 2000 + BCD_CHAR(t.year) - 1900;
    DEBUG("year %d",tmTime.tm_year);
    tmTime.tm_mon = BCD_CHAR(t.month) - 1;
    tmTime.tm_mday = BCD_CHAR(t.day) ;
    tmTime.tm_hour = BCD_CHAR(t.hour);
    tmTime.tm_min = BCD_CHAR(t.minute);
    tmTime.tm_sec = BCD_CHAR(t.second);
    DEBUG("set time as %s",asctime(&tmTime));
    tv = mktime(&tmTime);
    DEBUG("before inc time is %s",ctime(&tv));
    tv += nMinute*60;
    gmtime_r(&tv,&tmTime);
    DEBUG("after inc time is %s",ctime(&tv));
    t.year = CHAR_BCD(tmTime.tm_year + 1900 - 2000);
    t.month = CHAR_BCD(tmTime.tm_mon + 1);
    t.day  = CHAR_BCD(tmTime.tm_mday);
    t.hour = CHAR_BCD(tmTime.tm_hour);
    t.minute = CHAR_BCD(tmTime.tm_min);
    t.second = CHAR_BCD(tmTime.tm_sec);
}

void USBDataFile::Save(const char* szPath)
{
    FILE* f = fopen(szPath,"wb");
    if (f)
      {
      fwrite((const char*)pData,1,sizeof(USBFile),f);
      fclose(f);
      }

}
