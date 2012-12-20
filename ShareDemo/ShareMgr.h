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

//����ӿ� [LCM 2012/11/22  14:55]
public:
	//���̰߳�ȫ���� [LCM 2012/11/22  14:28]
	static CShareMemMgr* GetInstance();
	static void ReleaseMgr();

public:
	bool	InitShareMgr();
	bool	SendMsg(const void *pMsg);
	bool	SetRecvHandle(IMsgSink *pHandle);

	//�ڲ� [LCM 2012/11/22  14:54]
private:
	//�ڲ�����Ϣ [LCM 2012/11/22  14:26]	
	bool	RecvMsg(BYTE const *pMsg, unsigned int &nMsgSize);

	//�ڲ��߳���� [LCM 2012/11/22  14:26]
	friend static unsigned __stdcall Process(void * pThis);


	//�ⲿ֪ͨ���ƴ����� [LCM 2012/11/22  14:26]
	IMsgSink *m_Handle;

private:
	unsigned int	m_nError;
	
	HANDLE	m_hWriteEvent;	 //д�¼� [LCM 2012/11/19  9:29]
	HANDLE  m_hWriteMutex;
	HANDLE  m_hWriteFileMap;
	LPVOID  m_hWriteViewBuf; 

	HANDLE	m_hReadEvent;	 //���¼� [LCM 2012/11/19  9:29]
	HANDLE  m_hReadMutex;
	HANDLE  m_hReadFileMap; 
	LPVOID  m_hReadViewBuf;

    //������
    //std::vector<tagMsgBase> m_vecSend;
};
