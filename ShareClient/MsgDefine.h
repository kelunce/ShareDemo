#pragma  once

//��Ϣ����
const short MSG_TYPE_CMD=200;
const short MSG_TYPE_TEXT = 201;

//buff���ȶ���
const short _MAX_MSG_BUFF = 1024;                                   //ÿ����Ϣ�ĳ���,����
const int _MAX_SHARE_MSG = 1024*8;                                  //�ڴ�������,��Ϣ����Ϊ��λ
const int _MAX_SHARE_BYTES = _MAX_MSG_BUFF*_MAX_SHARE_MSG;          //�ڴ�������,�ֽڳ���Ϊ��λ

class IMsgSink
{
public:
	virtual void OnRecv(unsigned short msgType,void * pmsg) = 0;
};

//����ṹ
struct SHARE_MSG_BUFF 
{
	unsigned int nSize;
	BYTE szBuff[_MAX_SHARE_BYTES];
};

//��Ϣ�ṹ
struct tagMsgBase
{
    unsigned short unMsgType;
    BYTE    szBuff[_MAX_MSG_BUFF - 2];
};

struct tagMsgCommand:public tagMsgBase
{	
    struct MsgInfo
    {
        unsigned short unMsgType;
	    unsigned short unCmdID;
    }*m_pinfo;

    tagMsgCommand(unsigned short nCmd)
    {
        m_pinfo = (MsgInfo*)szBuff;
        m_pinfo->unMsgType = MSG_TYPE_CMD;
        m_pinfo->unCmdID = nCmd;
    }

};

struct tagMsgTalk:public tagMsgBase
{
    static const int _MAX_TEXT_SIZE = 256;

    struct MsgInfo
    {
        unsigned short unMsgType;
        TCHAR szText[_MAX_TEXT_SIZE];
    }*m_pinfo;

	tagMsgTalk(TCHAR *pWords,short nSize)
	{
        m_pinfo = (MsgInfo*)szBuff;
        m_pinfo->unMsgType = MSG_TYPE_TEXT;
		_stprintf_s(m_pinfo->szText,_MAX_TEXT_SIZE,_T("%s"),pWords);
	}
};


