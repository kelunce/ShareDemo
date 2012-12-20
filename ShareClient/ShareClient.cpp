// ShareClient.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "ShareClient.h"
#include "ShareMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

//���ջص� [LCM 2012/11/22  15:05]
class CMyShareMemSink:public IMsgSink
{
	void OnRecv(unsigned short msgType,void * pmsg)
	{
		//cout<<"recv:"<<"msgid="<<msgID<<" size="<<nSize<<endl;
		if(msgType == MSG_TYPE_CMD)
		{
			tagMsgCommand::MsgInfo *pCmd = (tagMsgCommand::MsgInfo *)pmsg;
			wcout<<_T("recv cmd id=")<< pCmd->unCmdID<<endl;
			return;
		}
		else if (msgType == MSG_TYPE_TEXT)
		{
            tagMsgTalk::MsgInfo *pTalk = (tagMsgTalk::MsgInfo *)pmsg;
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
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
		nRetCode = 1;
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
			Sleep(1000);

			if(rand()%100 > 50)			
			{
				short nCmdID = 800 + rand()%100;
				tagMsgCommand msg(nCmdID);
				pApiMgr->SendMsg(msg.m_pinfo);
				wcout<<_T("send cmd id=")<<nCmdID<<endl;
			}
			else
			{
				TCHAR *pWords = _T("��������,����������ô�ȹ�!");
				tagMsgTalk msg(pWords,wcslen(pWords));
				pApiMgr->SendMsg(msg.m_pinfo);
				wcout<<_T("send str=")<<pWords<<endl;
			}
		}
	}

	return nRetCode;
}
