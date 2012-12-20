// ShareClient.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ShareClient.h"
#include "ShareMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

//接收回调 [LCM 2012/11/22  15:05]
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

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		wcout.imbue(locale("chs"));
		CShareMemMgr* pApiMgr = CShareMemMgr::GetInstance();
		if(!pApiMgr->InitShareMgr())
		{
			MessageBox(NULL, _T("创建共享失败"), _T("Server"), MB_OK); 
		}

		//指定回调 [LCM 2012/11/22  15:06]
		CMyShareMemSink mySink;
		pApiMgr->SetRecvHandle(&mySink);

		//主线程去做其他的事情 [LCM 2012/11/22  14:56]
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
				TCHAR *pWords = _T("冬天来了,天气还是这么热哈!");
				tagMsgTalk msg(pWords,wcslen(pWords));
				pApiMgr->SendMsg(msg.m_pinfo);
				wcout<<_T("send str=")<<pWords<<endl;
			}
		}
	}

	return nRetCode;
}
