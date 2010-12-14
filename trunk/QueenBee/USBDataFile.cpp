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
  if (!p.GetSize()) return 0;
  SpeedRecord* pRec = p.GetSpeedData(n);
  if (!pRec) return 0;

  char* cache = (char*)pData;

  if ((n+sizeof(struct PacketHead)) < p.GetSize())
    {
      INFO("Data packet is not completed.");
       n = p.GetSize()-sizeof(struct PacketHead);
    }

  DEBUG("Total %d:%d data",n/sizeof(SpeedRecord),(n%sizeof(SpeedRecord))?0:(n%sizeof(SpeedRecord))-sizeof(struct RecordTime));

  int nStart = pData->table.BaseData.BaseAddr;
  int nEnd = pData->table.BaseData.EndAddr;
  int nCur = nStart;
  DEBUG("start address:0x%08X",nStart);
  RecordData_start* rec_start = (RecordData_start*)(cache+nStart);
  RecordData_end* rec_end;

  while (n>sizeof(struct RecordTime))
   {
    if ((nCur+sizeof(RecordData_start)+sizeof(RecordData_end)+8)>=nEnd) return 1;
    rec_start = (RecordData_start*)(cache+nCur);
    setStartTime(rec_start, pRec);
    nCur+=sizeof(RecordData_start);

    DEBUG("start at %d-%d-%d %d:%d",BCD_CHAR(pRec->startTime.bcdYear),
          BCD_CHAR(pRec->startTime.bcdMonth),
          BCD_CHAR(pRec->startTime.bcdDay),
          BCD_CHAR(pRec->startTime.bcdHour),
          BCD_CHAR(pRec->startTime.bcdMinute));

     int nNum = 0;

     if (n >= sizeof(SpeedRecord))
        {
          nNum = 60;
          n -= sizeof(SpeedRecord);
        }
      else
        {
          nNum = n - sizeof(struct RecordTime);
          n = 0;
        }

     int seconds=expandSpeedRecord(cache,nCur,nEnd,pRec,nNum);

      rec_end = (RecordData_end*) (cache + nCur);
      memcpy(rec_end,rec_start,sizeof(RecordData_start));
      incTime(rec_end->dt,seconds/60,seconds%60);
      rec_end->dt.type = 0xAFAF;
      nCur+=sizeof(RecordData_end)+8;
      pRec+=1;

      DEBUG("end at %d-%d-%d %d:%d",BCD_CHAR(rec_end->dt.year),
           BCD_CHAR(rec_end->dt.month),
           BCD_CHAR(rec_end->dt.day),
           BCD_CHAR(rec_end->dt.hour),
           BCD_CHAR(rec_end->dt.minute));
    };

  return 1;
}

int USBDataFile::expandSpeedRecord(char* cache,int& nCur,int nEnd,SpeedRecord* pRec,int nNum)
{
  int ii,m;
  for (ii = 0; ii < nNum; ii++)
   {
      for (m = 0; m < 60; m++)
        {
          if (((unsigned) (nCur + sizeof(RecordData_end) + 8)) >= ((unsigned) nEnd))
            {
              INFO("No space for data after %d:%d",ii,m);
              break;
            }
          cache[nCur++] = pRec->speed[ii];
          cache[nCur++] = 0;
        }
    }
   return ii*60+m;
}

void USBDataFile::setStartTime(RecordData_start* rec_start,SpeedRecord* pRec)
{
  if ((!rec_start) || (!pRec)) return;
  rec_start->dt.type = 0xFEFE;
  rec_start->dt.year = pRec->startTime.bcdYear;
  rec_start->dt.month = pRec->startTime.bcdMonth;
  rec_start->dt.day =  pRec->startTime.bcdDay;
  rec_start->dt.hour = pRec->startTime.bcdHour;
  rec_start->dt.minute = pRec->startTime.bcdMinute;
  rec_start->dt.second = 0;
}

void USBDataFile::incTime(Record_CLOCK& t, int nMinute,int nSecond)
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
    tv += nMinute*60+nSecond;
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