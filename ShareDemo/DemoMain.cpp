// DemoMain.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "DemoMain.h"
#include "ShareMgr.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 唯一的应用程序对象

CWinApp theApp;
using namespace std;

/************************************************************************/
//此工程的debug版本是控制台,release版本是windows程序,只是入口指定为main
//在创建此工程时是控制台,选择了mfc支持
//														by LCM 2012/11/22
/************************************************************************/


//接收回调 [LCM 2012/11/22  15:05]
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

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		//_tprintf(_T("错误: MFC 初始化失败\n"));
		nRetCode = 1;
		MessageBox(NULL, _T("启动失败"), TEXT("Server"),MB_OK); 
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
				TCHAR *pWords = _T("确实很坑爹");
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
