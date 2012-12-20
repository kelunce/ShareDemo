#pragma once

#include "MsgDefine.h "

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       {  try{if(p)   delete (p);     (p)=NULL; } catch(...){ p=NULL;}   }
#endif
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->ReleaseMgr(); (p)=NULL; } }

class CShareMemMgr
{
private:
	CShareMemMgr(void);
	~CShareMemMgr(void);

//对外接口 [LCM 2012/11/22  14:55]
public:
	//非线程安全单例 [LCM 2012/11/22  14:28]
	static CShareMemMgr* GetInstance();
	static void ReleaseMgr();

public:
	bool	InitShareMgr();
	bool	SendMsg(const void *pMsg);
	bool	SetRecvHandle(IMsgSink *pHandle);

	//内部 [LCM 2012/11/22  14:54]
private:
	//内部收信息 [LCM 2012/11/22  14:26]	
	bool	RecvMsg(BYTE const *pMsg, unsigned int &nMsgSize);

	//内部线程入口 [LCM 2012/11/22  14:26]
	friend static unsigned __stdcall Process(void * pThis);


	//外部通知控制处理器 [LCM 2012/11/22  14:26]
	IMsgSink *m_Handle;

private:
	unsigned int	m_nError;
	
	HANDLE	m_hWriteEvent;	 //写事件 [LCM 2012/11/19  9:29]
	HANDLE  m_hWriteMutex;
	HANDLE  m_hWriteFileMap;
	LPVOID  m_hWriteViewBuf; 

	HANDLE	m_hReadEvent;	 //读事件 [LCM 2012/11/19  9:29]
	HANDLE  m_hReadMutex;
	HANDLE  m_hReadFileMap; 
	LPVOID  m_hReadViewBuf;

    //缓冲区
    //std::vector<tagMsgBase> m_vecSend;
};
