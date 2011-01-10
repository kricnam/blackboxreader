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

int USBDataFile::AddAccidentData(Packet &p)
{
   unsigned int n;
   if (!p.GetSize()) return 0;
   struct AccidentData * pRec = p.GetAccidentData(n);
   if (!pRec) return 0;

   char* cache = (char*)pData;
   int nStart = pData->table.DoubtPointData.BaseAddr;
   int nEnd = pData->table.DoubtPointData.EndAddr;
   int nCur = nStart;
   DEBUG("start address:0x%08X",nStart);

   for(unsigned int i=0;i<n;i++)
   {
	   if ((unsigned)(nCur+sizeof(pData->para.DriverCode)+sizeof(struct AccidentData))> (unsigned)nEnd)
		   break;
	   memcpy(cache+nCur,&(pData->para.DriverCode),sizeof(pData->para.DriverCode));
	   nCur+=sizeof(pData->para.DriverCode);
	   memcpy(cache+nCur,&pRec[i],sizeof(struct AccidentData));
	   nCur+=sizeof(struct AccidentData);
   }
   return 1;
}

int USBDataFile::AddSpeedData(Packet &p)
{
  unsigned int n=0;
  if (!p.GetSize()) return 0;
  SpeedRecord* pRec = p.GetSpeedData(n);
  if (!pRec) return 0;

  nOdeMeter = 0;
  char* cache = (char*)pData;

  if ((n+sizeof(struct PacketHead))+1 > (unsigned)p.GetSize())
   {
      INFO("Data packet is not completed, n=%d, size=%d",n,p.GetSize());
      n = p.GetSize()-sizeof(struct PacketHead)-1;
   }

  DEBUG("Total %d:%d data",n/sizeof(SpeedRecord),((n%sizeof(SpeedRecord))==0)?0:(n%sizeof(SpeedRecord))-sizeof(struct RecordTime));

  int nStart = pData->table.BaseData.BaseAddr;
  int nEnd = pData->table.BaseData.EndAddr;
  int nCur = nStart;
  DEBUG("start address:0x%08X",nStart);
  RecordData_start start={{0}};
  int nRunSec = 0;
  while (n>sizeof(struct RecordTime))
   {
    if ((nCur+(int)sizeof(RecordData_start)+(int)sizeof(RecordData_end))>=nEnd)
    	break;

     int nNum = 0;

     if (n >= (int)sizeof(SpeedRecord))
        {
          nNum = 60;
          n -= sizeof(SpeedRecord);
        }
      else
        {
          nNum = n - sizeof(struct RecordTime);
          n = 0;
        }

     nRunSec = expandSpeedRecord(cache,nCur,nEnd,pRec,nNum,nRunSec,start);
     pRec+=1;

     if (nRunSec==0) continue;
     if (nRunSec && timeContinue(pRec,start,nRunSec)) continue;

     RecordData_end* rec_end = setEndTime(cache,nCur,start.dt);
     incTime(rec_end->dt,((nRunSec/60)==0)?0:(nRunSec/60)-1,(nRunSec)%60);
	 nRunSec = 0;

    };

  return 1;
}

bool USBDataFile::timeContinue(SpeedRecord* pRec,RecordData_start& rec_start, int nSeconds)
{
	RecordData_start time,now;
	time = rec_start;
	incTime(time.dt, nSeconds/60,nSeconds%60);
	int n = 0;
	setStartTime((char*)&now,n,&pRec->startTime);
	return (now.dt.year == time.dt.year &&
			now.dt.month == time.dt.month &&
			now.dt.day == time.dt.day &&
			now.dt.hour == time.dt.hour &&
			now.dt.minute == time.dt.minute &&
			now.dt.second == time.dt.second);
}

int USBDataFile::findStart(SpeedRecord* pRec,int&start,int nNum)
{
	int n = start;
	while(n < nNum && pRec->speed[n]==0 ) {n++;};
	if (n==nNum) {start = n;return 0;}
	start = n;
	n = 0;
	while(((start+n)<nNum) && pRec->speed[start+n] ) {n++;};
	return n;
}

int USBDataFile::fillData(char* cache,int& nCur,int nEnd,char* data,int nNum)
{
	int n=0;
	int i=0;
	double distince = 0;
	int nSec = 0;
	for(i=0;i<nNum;i++)
	{
		distince += data[i]/60.0;
		for(n=0;n<60;n++)
		{
			if (((unsigned) (2 + nCur + sizeof(RecordData_end) )) >= ((unsigned) nEnd))
			{
			       INFO("No space for data after %d:%d",i,n);
			       nNum = i;
			       distince -= data[i]/3600.0 * (60 - n);
			       break;
			}
			cache[nCur++]=data[i];
			cache[nCur++]=0;
			nSec++;
		};
	};
	nOdeMeter += (int)distince;
	return nSec;
}

USBDataFile::RecordData_start* USBDataFile::setStartTime(char* cache,int& nCur,RecordTime* pTime)
{

  DEBUG("%d-%d-%d %d:%d",BCD_CHAR(pTime->bcdYear),
                BCD_CHAR(pTime->bcdMonth),
                BCD_CHAR(pTime->bcdDay),
                BCD_CHAR(pTime->bcdHour),
                BCD_CHAR(pTime->bcdMinute));
	  RecordData_start* rec_start = (RecordData_start* )(cache+nCur);
	  rec_start->dt.type = 0xFEFE;
	  rec_start->dt.year = pTime->bcdYear;
	  rec_start->dt.month = pTime->bcdMonth;
	  rec_start->dt.day =  pTime->bcdDay;
	  rec_start->dt.hour = pTime->bcdHour;
	  rec_start->dt.minute = pTime->bcdMinute;
	  rec_start->dt.second = 0;
	  nCur+=sizeof(RecordData_start);
	  return rec_start;
}

USBDataFile::RecordData_end* USBDataFile::setEndTime(char* cache,int& nCur,Record_CLOCK& start)
{
  RecordData_end* rec_end = (RecordData_end* )(cache+nCur);
  rec_end->dt.type = 0xAEAE;
  rec_end->dt.year = start.year;
  rec_end->dt.month = start.month;
  rec_end->dt.day =  start.day;
  rec_end->dt.hour = start.hour;
  rec_end->dt.minute = start.minute;
  rec_end->dt.second = start.second;
  rec_end->DriverCode = pData->para.DriverCode;
  rec_end->DistancePulse = 8*nOdeMeter*pData->para.CHCO ;
  nOdeMeter = 0;
  nCur+=sizeof(RecordData_end);
  DEBUG("%d-%d-%d %d:%d:%d",BCD_CHAR(rec_end->dt.year),
                  BCD_CHAR(rec_end->dt.month),
                  BCD_CHAR(rec_end->dt.day),
                  BCD_CHAR(rec_end->dt.hour),
                  BCD_CHAR(rec_end->dt.minute),BCD_CHAR(rec_end->dt.second));

  return rec_end;
}

int USBDataFile::expandSpeedRecord(char* cache,int& nCur,int nEnd,SpeedRecord* pRec,int nNum,int nRunSecond,RecordData_start& lastStart)
{
  RecordData_start* rec_start;
  RecordData_end* rec_end;

  int start=0;
  int n=0;
  int nSecond;
  while((n = findStart(pRec,start,nNum)))
  {
	  if ((nCur+1+(int)sizeof(RecordData_start)+(int)sizeof(RecordData_end))>=nEnd)
	    	break;
	  DEBUG("Runing from minute %d, total %d minute",start,n);
	  if (!nRunSecond)//0
	  {
		  rec_start = setStartTime(cache,nCur,&(pRec->startTime));
		  incTime(rec_start->dt,start,0);
	  }
	  else
	  {
		  DEBUG("Runing not stop on last record");
		  if (start)//stoped at first minute
		  {
			  rec_end = setEndTime(cache,nCur,lastStart.dt);
			  incTime(rec_end->dt,(nRunSecond/60)?(nRunSecond/60)-1:0,nRunSecond%60);
			  rec_start = setStartTime(cache,nCur,&(pRec->startTime));
			  incTime(rec_start->dt,start,0);
			  nRunSecond = 0;
		  }
		  else
		  {
			  DEBUG("stoped at %d",n);
			  rec_start = &lastStart;
		  }
	  }

	  nSecond = fillData(cache,nCur,nEnd,(char*)&(pRec->speed[start]),n);
	  if (start+n>= nNum)
	  {
		  lastStart = *rec_start;
		  return nSecond + nRunSecond;
	  }

	  rec_end = setEndTime(cache,nCur,rec_start->dt);
	  incTime(rec_end->dt,((nRunSecond+nSecond)/60==0)?0:((nRunSecond+nSecond)/60)-1,(nRunSecond+nSecond)%60);

	  start+=n;
  };

  return nRunSecond;
}

void USBDataFile::incTime(Record_CLOCK& t, int nMinute,int nSecond)
{
    time_t tv;
    struct tm tmTime={0};
    tmTime.tm_year = 2000 + BCD_CHAR(t.year) - 1900;
    tmTime.tm_mon = BCD_CHAR(t.month) - 1;
    tmTime.tm_mday = BCD_CHAR(t.day) ;
    tmTime.tm_hour = BCD_CHAR(t.hour);
    tmTime.tm_min = BCD_CHAR(t.minute);
    tmTime.tm_sec = BCD_CHAR(t.second);

    tv = mktime(&tmTime);

    tv += nMinute*60+nSecond;
    gmtime_r(&tv,&tmTime);

    t.year = CHAR_BCD(tmTime.tm_year + 1900 - 2000);
    t.month = CHAR_BCD(tmTime.tm_mon + 1);
    t.day  = CHAR_BCD(tmTime.tm_mday);
    t.hour = CHAR_BCD(tmTime.tm_hour);
    t.minute = CHAR_BCD(tmTime.tm_min);
    t.second = CHAR_BCD(tmTime.tm_sec);
    DEBUG("After: %d-%d-%d %d:%d:%d",BCD_CHAR(t.year),
                      BCD_CHAR(t.month),
                      BCD_CHAR(t.day),
                      BCD_CHAR(t.hour),
                      BCD_CHAR(t.minute),BCD_CHAR(t.second));

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
