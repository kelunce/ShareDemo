#pragma  once

//消息类型
const short MSG_TYPE_CMD=200;
const short MSG_TYPE_TEXT = 201;

//buff长度定义
const short _MAX_MSG_BUFF = 1024;                                   //每个消息的长度,定长
const int _MAX_SHARE_MSG = 1024*8;                                  //内存中容量,消息长度为单位
const int _MAX_SHARE_BYTES = _MAX_MSG_BUFF*_MAX_SHARE_MSG;          //内存中容量,字节长度为单位

class IMsgSink
{
public:
   virtual void OnRecv(unsigned short msgType,void * pmsg) = 0;
};

//共享结构
struct SHARE_MSG_BUFF 
{
    unsigned int nSize;
    BYTE szBuff[_MAX_SHARE_BYTES];
};

//消息结构
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


