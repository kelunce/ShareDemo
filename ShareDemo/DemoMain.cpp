// DemoMain.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "DemoMain.h"
#include "ShareMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;
using namespace std;

/************************************************************************/
//�˹��̵�debug�汾�ǿ���̨,release�汾��windows����,ֻ�����ָ��Ϊmain
//�ڴ����˹���ʱ�ǿ���̨,ѡ����mfc֧��
//														by LCM 2012/11/22
/************************************************************************/


//���ջص� [LCM 2012/11/22  15:05]
class CMyShareMemSink:public IMsgSink
{
	void OnRecv(short msgID,void * pmsg,short nSize)
	{
		//cout<<"recv:"<<"msgid="<<msgID<<" size="<<nSize<<endl;
		if(msgID == MSG_CMD_TYPE)
		{
			MsgCommand *pCmd = (MsgCommand *)pmsg;
			wcout<<_T("recv cmd id=")<<pCmd->cmdID<<endl;
			return ;
		}
		else if (msgID == MSG_TEXT)
		{
			MsgTalk *pTalk = (MsgTalk *)pmsg;
			wcout<<_T("recv words=")<<pTalk->szText<<endl;
			return ;
		}

		wcout<<_T("recv unknow msg")<<endl;		
	};
};



int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{	
	int nRetCode = 0;

	// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		//_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
		nRetCode = 1;
		MessageBox(NULL, _T("����ʧ��"), TEXT("Server"),MB_OK); 
	}
	else
	{	
		wcout.imbue(locale("chs"));
		CShareMemMgr* pApiMgr = CShareMemMgr::GetInstance();
		if(!pApiMgr->InitShareMgr())
		{
			MessageBox(NULL, _T("��������ʧ��"), _T("Server"), MB_OK); 
		}

		//ָ���ص� [LCM 2012/11/22  15:06]
		CMyShareMemSink mySink;
		pApiMgr->SetRecvHandle(&mySink);

		//���߳�ȥ������������ [LCM 2012/11/22  14:56]
		while(true)
		{
			Sleep(5000);			

			DWORD dwTime = GetTickCount();
			if(rand()%100 > 50)			
			{
				short nCmdID= 400 + rand()%100;
				MsgCommand msg(nCmdID);
				if(pApiMgr->SendMsg(&msg,sizeof(msg)))
					wcout<<dwTime<<_T(": send cmd id=")<<nCmdID<<"    ok"<<endl;
				else
					wcout<<dwTime<<_T(": send cmd id=")<<nCmdID<<"    fail"<<endl;
			}
			else
			{
				TCHAR *pWords = _T("ȷʵ�ܿӵ�");
				MsgTalk msg(pWords,wcslen(pWords));
				if(pApiMgr->SendMsg(&msg,sizeof(msg)))
					wcout<<dwTime<<_T(": send str=")<<pWords<<"     ok"<<endl;
				else
					wcout<<dwTime<<_T(": send str=")<<pWords<<"     fail"<<endl;
			}
		}
	}

	return nRetCode;
}
