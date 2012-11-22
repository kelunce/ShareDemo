#include "stdafx.h"
#include "ShareMgr.h"


static CShareMemMgr* g_pShareMemMgr = NULL;
const int _MAX_SHARE_BYTES = 2048;
const TCHAR * szReadEventName = _T("8257D348B9C645cfA79E59788E04F84B");
const TCHAR * szReadMutex = _T("132E0E5A25D946298C90FAE8821D7E18");
const TCHAR * szReadMemeryName = _T("FEF2AEC802494efeA1B2393B3316C9A9");
const TCHAR * szWriteEventName = _T("F03A83039A434e908C426210E3292477");
const TCHAR * szWriteMutex = _T("09085E5D7BA84248B1D370DADB8B9B9F");
const TCHAR * szWriteMemeryName = _T("A234003C644D49359A53D2B871D237C2");



CShareMemMgr* CShareMemMgr::GetInstance()
{
	if(g_pShareMemMgr==NULL)
	{
		g_pShareMemMgr = new CShareMemMgr();
	}

	return g_pShareMemMgr;
}

void CShareMemMgr::ReleaseMgr()
{
	SAFE_DELETE(g_pShareMemMgr);
	g_pShareMemMgr = NULL;
}

CShareMemMgr::CShareMemMgr(void)
{
}

CShareMemMgr::~CShareMemMgr(void)
{
	CloseHandle(m_hWriteEvent);
	CloseHandle(m_hWriteFileMap);
	CloseHandle(m_hWriteMutex);
	CloseHandle(m_hReadMutex);
	CloseHandle(m_hWriteFileMap);
	CloseHandle(m_hReadEvent);
}


bool CShareMemMgr::InitShareMgr()
{
	m_hWriteEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,szWriteEventName);
	if (m_hWriteEvent == NULL)
	{
		return false;
	}

	m_hWriteMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,szWriteMutex);
	if (m_hWriteMutex == NULL)
	{		
		return false;
	}

	m_hReadEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,szReadEventName);
	if (m_hReadEvent == 0)
	{
		return false;
	}

	m_hReadMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,szReadMutex);
	if (m_hReadMutex == 0)
	{
		return false;
	}
	m_hWriteFileMap = OpenFileMapping(FILE_MAP_WRITE,FALSE,szWriteMemeryName);
	if (m_hWriteFileMap == 0)
	{
		return false;
	}

	m_hWriteViewBuf = MapViewOfFile(m_hWriteFileMap,FILE_MAP_WRITE,0,0,_MAX_SHARE_BYTES);

	m_hReadFileMap = OpenFileMapping(FILE_MAP_READ,FALSE,szReadMemeryName);
	if (m_hReadFileMap == 0)
	{
		return false;
	}
	m_hReadViewBuf = MapViewOfFile(m_hReadFileMap,FILE_MAP_READ,0,0,_MAX_SHARE_BYTES);


	//Æô¶¯¼àÌýÏß³Ì [LCM 2012/11/22  14:54]
	_beginthreadex(NULL,0,Process,(void*)this,0,0);
	

	return true;
}

bool CShareMemMgr::SendMsg( const void *pMsg, unsigned int nMsgSize )
{
	if (pMsg != NULL)
	{
		if(nMsgSize > _MAX_MSG_BUFF)
			return false;

		if (m_hWriteViewBuf == NULL)
			return false;

		if (nMsgSize == 0)
			return false;

		int nTryTimes =  3;
		while (nTryTimes > 0)
		{
			DWORD dwRet = WaitForSingleObject(m_hWriteMutex, 10);
			if(dwRet == WAIT_OBJECT_0)
			{
				SHARE_MSG_BUFF * pBuff = (SHARE_MSG_BUFF *)m_hWriteViewBuf;
				pBuff->nSize = nMsgSize;
				CopyMemory(pBuff->szBuff,pMsg,nMsgSize);
				SetEvent(m_hWriteEvent);
				ReleaseMutex(m_hWriteMutex);
				return true;
			}
			else if(dwRet == WAIT_ABANDONED)
			{
				ReleaseMutex(m_hWriteMutex);
			}
			nTryTimes--;
		}
	}
	return false;
}

bool CShareMemMgr::RecvMsg( TCHAR *pMsg, unsigned int &nMsgSize )
{
	if (pMsg != NULL)
	{
		if(nMsgSize > _MAX_MSG_BUFF)
			return false;

		 if(WaitForSingleObject(m_hReadEvent, 10) == WAIT_OBJECT_0)
		 {
			 int nTryTime = 3;
			 while(nTryTime > 0)
			 {
				 DWORD  dwMutex = WaitForSingleObject(m_hReadMutex, 0);
				 if(dwMutex == WAIT_OBJECT_0)
				 {
					 SHARE_MSG_BUFF * pBuff = (SHARE_MSG_BUFF *)m_hReadViewBuf;
					 nMsgSize = pBuff->nSize;
					 CopyMemory(pMsg,pBuff->szBuff,nMsgSize);
					 ReleaseMutex(m_hReadMutex);
					 return true;
				 }		
				 else if (dwMutex == WAIT_ABANDONED)
				 {
					 ReleaseMutex(m_hReadMutex);
				 }
				 nTryTime--;
			 }
		 }
	}
	return false;
}

unsigned __stdcall Process(void *pThis)
{
	if(pThis == NULL)
		return 1;

	CShareMemMgr *pMgr = (CShareMemMgr *)pThis;
	while(true)
	{
		TCHAR szMsg[_MAX_SHARE_BYTES] = {0};
		unsigned int nSize = 0;
		if(pMgr->RecvMsg(szMsg,nSize))
		{
			const short *pMsgID = (const short *)szMsg;
			if( (*pMsgID <= 0 )|| (*pMsgID >1024))
				continue;

			//msg process [LCM 2012/11/22  13:53]
			if(pMgr->m_Handle != NULL)
				pMgr->m_Handle->OnRecv(*pMsgID,szMsg,nSize);
		}
	}
	return 1;
}

bool CShareMemMgr::SetRecvHandle( IMsgSink *pHandle )
{
	if(pHandle==NULL)
		return false;
	m_Handle = pHandle;
	return true;
};	

