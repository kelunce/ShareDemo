#include "stdafx.h"
#include "ShareMgr.h"


static CShareMemMgr* g_pShareMemMgr = NULL;

const TCHAR * szWriteEventName = _T("8257D348B9C645cfA79E59788E04F84B");
const TCHAR * szWriteMutex = _T("132E0E5A25D946298C90FAE8821D7E18");
const TCHAR * szWriteMemeryName = _T("FEF2AEC802494efeA1B2393B3316C9A9");
const TCHAR * szReadEventName = _T("F03A83039A434e908C426210E3292477");
const TCHAR * szReadMutex = _T("09085E5D7BA84248B1D370DADB8B9B9F");
const TCHAR * szReadMemeryName = _T("A234003C644D49359A53D2B871D237C2");


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
	m_hWriteEvent = CreateEvent(NULL,FALSE,FALSE,szWriteEventName);
	if (GetLastError() != 0)
	{
		return false;
	}
	
	m_hWriteMutex = CreateMutex(NULL,FALSE,szWriteMutex);

	if (GetLastError() != 0)
	{		
		return false;
	}
	
	m_hReadEvent = CreateEvent(NULL,FALSE,FALSE,szReadEventName);
	if (GetLastError() != 0)
	{
		
		return false;
	}

	m_hReadMutex = CreateMutex(NULL,FALSE,szReadMutex);
	if (GetLastError() != 0)
	{
		return false;
	}

	m_hWriteFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,	0,_MAX_SHARE_BYTES,szWriteMemeryName);
	if (GetLastError() != 0)
	{
		return false;
	}

	m_hWriteViewBuf = MapViewOfFile(m_hWriteFileMap,FILE_MAP_ALL_ACCESS,0,0,_MAX_SHARE_BYTES);

	m_hReadFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,	0,_MAX_SHARE_BYTES,szReadMemeryName);
	if (GetLastError() != 0)
	{
		return false;
	}
    m_hReadViewBuf = MapViewOfFile(m_hReadFileMap,FILE_MAP_ALL_ACCESS,0,0,_MAX_SHARE_BYTES);

	//���������߳� [LCM 2012/11/22  14:54]
	_beginthreadex(NULL,0,Process,(void*)this,0,0);
	

	return true;
}

bool CShareMemMgr::SendMsg(const void *pMsg)
{
	if (pMsg != NULL)
	{
		if (m_hWriteViewBuf == NULL)
			return false;		
		
		DWORD dwRet = WaitForSingleObject(m_hWriteMutex, 30);
		if(dwRet == WAIT_OBJECT_0)
		{
			SHARE_MSG_BUFF * pBuff = (SHARE_MSG_BUFF *)m_hWriteViewBuf;
            if(pBuff->nSize >= _MAX_SHARE_MSG - 1)
            {
                ReleaseMutex(m_hWriteMutex);
                return false;
            }

            /*�Ż�����:
                1.���︴����_MAX_MSG_BUFF�Ĵ�С,���Խ�һ���Ż�,ȥ�����ิ��.
                2.��������㽨�����ͻ�����.ÿ�λ�û�����ʱ,������д�뼸����Ϣ,������ÿ��ֻд��һ��*/
            CopyMemory(pBuff->szBuff + pBuff->nSize * _MAX_MSG_BUFF,pMsg,_MAX_MSG_BUFF);
			pBuff->nSize += 1;
			
			SetEvent(m_hWriteEvent);
			ReleaseMutex(m_hWriteMutex);
			return true;
		}
		else if(dwRet == WAIT_ABANDONED)
		{
			ReleaseMutex(m_hWriteMutex);
		}			
		
	}
	return false;
}

bool CShareMemMgr::RecvMsg( BYTE const *pMsg, unsigned int &nMsgSize )
{
    nMsgSize = 0;

    if (pMsg == NULL)
    {
        return false;
    }

    if(WaitForSingleObject(m_hReadEvent, 10) == WAIT_OBJECT_0)
    {        
        DWORD  dwMutex = WaitForSingleObject(m_hReadMutex, 30);
        if(dwMutex == WAIT_OBJECT_0)
        {
            SHARE_MSG_BUFF * pBuff = (SHARE_MSG_BUFF *)m_hReadViewBuf;
            nMsgSize = pBuff->nSize;
            CopyMemory((void*)pMsg,pBuff->szBuff,nMsgSize*_MAX_MSG_BUFF);

            pBuff->nSize = 0;
            ReleaseMutex(m_hReadMutex);
            return true;
        }
        else if (dwMutex == WAIT_ABANDONED)
        {
            ReleaseMutex(m_hReadMutex);
        }      
    }

	return false;
}

unsigned __stdcall Process(void *pThis)
{
	if(pThis == NULL)
		return 1;

    /*ע��:
      1.��ʹ��byte,��Ҫʹ��TCHAR,���������ָ�������᲻����.
        ��unicode������,TCHAR��ռ2���ֽ�,ÿ�ƶ�һ����λָ��
        Ҳ�����ƶ�2���ֽ�.
      2.��ʹ�ö��ڴ�,����ܿ���ջ���.Ĭ�ϵ�ռ��СΪ1M 
    */
    BYTE const *szMsg = new BYTE[_MAX_SHARE_BYTES];
    unsigned int nSize = 0;
	CShareMemMgr *pMgr = (CShareMemMgr *)pThis;
	while(true)
	{
		if(!pMgr->RecvMsg(szMsg,nSize)) continue;

        for (int i = 0; i < nSize ; ++i)
		{
            tagMsgBase *pBuff = (tagMsgBase*)(szMsg + i * _MAX_MSG_BUFF);
            if(pMgr->m_Handle != NULL)
                pMgr->m_Handle->OnRecv(pBuff->unMsgType,(void*)pBuff);
		}
	}
    delete [] szMsg;
	return 1;
}

bool CShareMemMgr::SetRecvHandle( IMsgSink *pHandle )
{
	if(pHandle==NULL)
		return false;
	m_Handle = pHandle;
	return true;
};	

