#pragma  once

const short _MAX_MSG_BUFF = 1024;
const short MSG_CMD_TYPE=200;
const short MSG_TEXT = 201;

class IMsgSink
{
public:
	virtual void OnRecv(short msgID,void * pmsg,short nSize) = 0;
};


struct SHARE_MSG_BUFF 
{
	unsigned int nSize;
	BYTE szBuff[2000];
};

struct MsgCommand
{
	MsgCommand(short nCmd)
	{
		msgID = MSG_CMD_TYPE;
		cmdID = nCmd;
	}

	short msgID;
	short cmdID;
};

struct MsgTalk
{
	MsgTalk(TCHAR *pWords,short nSize)
	{
		msgID = MSG_TEXT;
		_stprintf_s(szText,256,_T("%s"),pWords);
	}
	short msgID;
	TCHAR szText[256];
};


