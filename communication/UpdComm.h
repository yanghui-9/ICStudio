#ifndef UPDCOMM_H
#define UPDCOMM_H
#include "communicationdef.h"
#include "LogService.h"

class UpdComm : public ICommunication
{
public:
    UpdComm();
    virtual ~UpdComm(){}


    virtual int32_t SendData(char * pBuffer, uint32_t iLength);

    virtual int32_t RecvData(char * pBuffer, int32_t  &iLength,uint32_t uiDelayTime);

    virtual int32_t AsynWR(char *sendBuf,int len,const ResponseCallBack &cb,
                           int readLenMax,int timeout);

    virtual int32_t AsynProcess(int32_t iLock = -1);

    virtual int32_t OpenComm(int setnonblock = 0,int Timo = 500,
                             const ConnectCB &connectCB = nullptr,
                             const DisconnectCB &disconnectCB = nullptr,
                             const ReadReadyCB &ReadReadyCB = nullptr);

    virtual int32_t CloseComm();

    virtual int32_t SetCommInfo(const std::string &sPar);

    virtual int32_t GetCommInfo(std::string &sPar);


    //日志
    virtual int32_t OpenLog(const char * LogName,int AscOrDec =0,int OpenType = 2);

    virtual int32_t CloseLog();

    int32_t WriteLog(const char *buf, int len, const char *fmt,...);

private:
    LogService m_log;
};

#endif // UPDCOMM_H
