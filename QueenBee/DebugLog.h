/*
 * DebugLog.h
 *
 *  Created on: 2010-1-11
 *      Author: mxx
 */
////////////////////////////////////////////////////////


#ifndef DEBUGLOG_H_
#define DEBUGLOG_H_
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <string>

using namespace std;

struct Location
{
	const char* szFile;
	const char* szFunc;
	const char* szLine;
};

#define LP_NONE           0
#define LP_TRACE          1
#define LP_DEBUG          2
#define LP_INFO           3
#define LP_NOTICE         4
#define LP_WARNING	 	  5
#define LP_ERROR          6
#define LP_CRITICAL       7
#define LP_ALERT          8
#define LP_EMERGENCY      9
#define LP_UNINITIALIZED  -1

class TraceLog
{
public:

	TraceLog(void);
	TraceLog(const char* szIP,int nPort);
	TraceLog(const char* szT,const char* szIP,int nPort);
	~TraceLog();

	void Trace(int nLevel, const char* szFile, const char* szFunc,int nLine, const char* szFmt,...);
	void Dump(int nLev, const char* szFile, const char* szFunc ,int nLine,const char* buf,int len);
	static void Init(const char* Title,const char* szIP, int nPort);

	static inline void SetLocalOut(bool bEnable) {bLocal = bEnable;}
	static void SetTraceLevel(int n) { nLevel = n;}
protected:
	static void SetTitle(char* szBuf,int nSize);		 //send out Date,Time and Title message
	void SendOut(const char* szBuf);

	static int nLogPort;
	static struct sockaddr_in sin;
	static int sct;
	static string strTitle;
	static char szLogIP[50];

	static const string priorityNames[];
	static int nLevel;
	static bool bLocal;
	static int nCounter;
};

extern TraceLog gTraceLog;


#define TraceLogInit(x,y,z)	gTraceLog.Init(x,y,z)

#define TRACE(args...) 		gTraceLog.Trace(LP_TRACE,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define DEBUG(args...) 		gTraceLog.Trace(LP_DEBUG,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define INFO(args...) 		gTraceLog.Trace(LP_INFO,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define NOTICE(args...) 	gTraceLog.Trace(LP_NOTICE,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define WARNING(args...)	gTraceLog.Trace(LP_WARNING,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define ERROR(args...) 		gTraceLog.Trace(LP_ERROR,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define CRITICAL(args...)	gTraceLog.Trace(LP_CRITICAL,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define ALERT(args...) 		gTraceLog.Trace(LP_ALERT,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define EMERGENCY(args...) 	gTraceLog.Trace(LP_EMERGENCY,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,args)
#define DUMP(buf,len)		gTraceLog.Dump(LP_TRACE,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,buf,len)
#define SETLOCALOUT(x)		gTraceLog.SetLocalOut(x)
#define SETTRACELEVEL(x)	gTraceLog.SetTraceLevel(x)
//TODO: make it thread safe
#define ERRTRACE()	{gTraceLog.Trace(LP_ERROR,(const char*)__FILE__,(const char*)__FUNCTION__,__LINE__,strerror(errno));}


#endif /* DEBUGLOG_H_ */
