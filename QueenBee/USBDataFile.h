/*
 * USBDataFile.h
 *
 *  Created on: 2010-4-29
 *      Author: mxx
 */

#ifndef USBDATAFILE_H_
#define USBDATAFILE_H_
/* 定义数据分区表 */
#define DOUBTPOINTDATA_EN               1
#define OVERSPEEDRECORD_EN      0
#define POWEROFFRUNRECORD_EN    0
#define RUNRECORD360h_EN                1
#define BASEDATA_EN                     1
#define OILDATA_EN                      0
#if RPM_EN
#define RPMDATA_EN                      1
#else
#define RPMDATA_EN                              0
#endif

#define TEMPERATUREDATA_EN      0
#define OILPRESSDATA_EN                 0

#define DOUBTPOINTDATA  	0
#define OVERSPEEDRECORD    	1
#define POWEROFFRUNRECORD   2
#define RUNRECORD360h 		3
#define BASEDATA 			4
#define OILDATA  			5
#define RPMDATA  			6
#define TEMPERATUREDATA  	7
#define OILPRESSDATA 		8

#define PartitionTableFlag  (DOUBTPOINTDATA_EN << DOUBTPOINTDATA)|(RUNRECORD360h_EN << RUNRECORD360h)|(BASEDATA_EN << BASEDATA)|(RPMDATA_EN << RPMDATA)

#define DATAFLASH_BASE      (0x00000000)
#define PARAMETER_BASE ((DATAFLASH_BASE+0x00000000))
#define PartitionTable_BASE ((DATAFLASH_BASE+0x00001000))
#define DPD_BASE      DATAFLASH_BASE+0x02000
#define DPD_END       DATAFLASH_BASE+0x06fff
#define RR360H_BASE   DATAFLASH_BASE+0x07000
#define RR360H_END    DATAFLASH_BASE+0x0cfff
#define BASEDATA_BASE DATAFLASH_BASE+0x0d000
#define BASEDATA_END  DATAFLASH_BASE+0xfffff

#ifndef u_char
typedef unsigned char u_char;
#endif

#ifndef u_short
typedef unsigned short u_short;
#endif

#ifndef u_int
typedef unsigned int u_int;
#endif

//#include "Packet.h"
class Packet;
struct SpeedRecord;


class USBDataFile {
public:
	USBDataFile();
	virtual ~USBDataFile();

	typedef struct
	{//定义时钟

		unsigned char year;
		unsigned char month;
		unsigned char day;
		unsigned char hour;
		unsigned char minute;
		unsigned char second;
		unsigned short dummy;

	} __attribute__ ((packed)) CLOCK;

	typedef struct
	{
		unsigned char speed;
		unsigned int bound;
	} __attribute__ ((packed)) RoadSection;

	typedef struct
	{
		unsigned short mark;//*特征字——2
		unsigned char  sn[22];//产品序列号——22(24)
		unsigned int CHCO;//车辆特征系数——4(28)
		unsigned char  AutoType[12];//车辆类型——12(40)
		unsigned char  AutoVIN[18];//车辆VIN号——18(58)
		unsigned char  AutoCode[12];//车牌号——12(70)
		unsigned char  AutoSort[12];//车牌分类——12(82)
		unsigned short  CodeColor;//车牌颜色——2(84)
		unsigned int  DriverCode;//驾驶员代码——4(88)
		unsigned char  DriverLisenseCode[20];//驾驶证号码——20(108)
		unsigned short status_polarity;//状态极性——2(110)
		unsigned short status_mask;//在用状态——2(112)
		unsigned char  OverSpeedTimeLimit;//超速时间门限（0－255秒）——1(113)
		unsigned char  AlarmSound;//声音报警选择（0x00-无声；0xFF-有声）——1(114)
		unsigned char  LowSpeedLimit;//低速路速度门限——1(115)
		unsigned char  HighSpeedLimit;//高速路速度门限——1(116)
		CLOCK   time;//实时时钟BCD码表示——8(124)
		CLOCK   InstallTime;//初装日期BCD码表示——8(132)
		unsigned char  PulseNumber;//车速传感器每转产生的脉冲数
		unsigned char  RPM_Pulse;
		unsigned char  Door1Type;
		unsigned char  Door2Type;
		unsigned char  reserved[118];//××（预留，暂时不用）(254)
		unsigned short  IBBType;//记录仪代码——2(256) （0x0A＝IBB-100A； 0x0C＝IBB-2C；0x30=IBB-3）
		unsigned char SectionNumber;
		RoadSection section[10];
	} __attribute__ ((packed)) StructPara;

	typedef struct
	{//分区表结构

		unsigned int  BaseAddr;//起始地址
		unsigned int  EndAddr;//结束地址
		unsigned int  CurPoint;//当前指针地址,指向下一个数据可存放的位置

	} __attribute__ ((packed)) StructPT;

	typedef struct
	{
		unsigned short  Available;//=0表示没有使用此数据区；>0表示数据区有效
		unsigned short  dummy;
		StructPT DoubtPointData;
		StructPT OverSpeedRecord;
		StructPT PowerOffRunRecord;
		StructPT RunRecord360h;
		StructPT BaseData;
		StructPT OilData;
		StructPT RPMData;
		StructPT TemperatureData;
		StructPT OilPressData;
		unsigned int	 TotalDistance;
		CLOCK	 LastUploadTime;
		unsigned int	 DriverCode;//驾驶员代码
		unsigned char   DriverLisenseCode[20];//驾驶证号码
		unsigned char 	 InOSAlarmCycle;//“在分路段报警周期中”标志
		unsigned int	 OSAlarmAddupDistance;//分路段报警路程累计

	} __attribute__ ((packed)) PartitionTable;

        typedef struct
        {//定义时钟

                u_short type;
                u_char year;
                u_char month;
                u_char day;
                u_char hour;
                u_char minute;
                u_char second;

        } __attribute__ ((packed)) Record_CLOCK;
        typedef struct
        {
                u_int  DriverCode;//驾驶员代码——4(88)
                u_char  DriverLisenseCode[20];//驾驶证号码——20(108)
        } __attribute__ ((packed)) DRIVER;
        typedef struct{
                Record_CLOCK  dt;
        } __attribute__ ((packed))OTDR_start;
        typedef struct{
                Record_CLOCK  dt;
                u_int  TotalDistance;
                u_int MinuteNb;
                DRIVER driver;
        } __attribute__ ((packed))OTDR_end;
        typedef struct{
                OTDR_start start;
                OTDR_end end;
        } __attribute__ ((packed))OTDR;
        typedef struct
        {//行驶记录
                Record_CLOCK dt;
        } __attribute__ ((packed)) RecordData_start;
        typedef struct
        {//行驶记录
                Record_CLOCK dt;
                u_int DistancePulse;
                u_int DriverCode;
                char tail[8];
        } __attribute__ ((packed)) RecordData_end;

	typedef struct
	{
		StructPara para;
		char para_gap[PartitionTable_BASE - sizeof(StructPara)];
		PartitionTable table;
		char table_gap[DPD_BASE-PartitionTable_BASE-sizeof(PartitionTable)];
		char data[1024*1024 - 0x02000];
	} __attribute__ ((packed)) USBFile;

	void Init();
	void InitPara(StructPara& para);
	int AddSpeedData(Packet &p);
	void Save(const char* szPath);
protected:
	void incTime(Record_CLOCK& t, int nMinute,int nSecond);
	void setStartTime(RecordData_start* rec_start,SpeedRecord* pRec);
	void setEndTime(RecordData_end* rec_end,SpeedRecord* pRec);
	int expandSpeedRecord(char* cache,int& nCur,int nEnd,SpeedRecord* pRec,int nNum);
	int fillData(char* cache,int& nCur,int nEnd,char* data,int nNum);
	int findStart(SpeedRecord* pRec,int&start,int nNum);
	USBFile *pData;

};

#endif /* USBDATAFILE_H_ */
