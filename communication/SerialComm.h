#ifndef SERIALPORT_H
#define SERIALPORT_H
#include "communicationdef.h"
#include "LogService.h"
#include "serialport/SerialPort.h"
using namespace itas109;
#define CACHE_BUFF_MAX (4096)

class SerialComm : public ICommunication
{
public:
    SerialComm(const std::string &sPar);
    virtual ~SerialComm();

    virtual int32_t SendData(char * pBuffer,  uint32_t iLength);

    virtual int32_t RecvData(char * pBuffer,  int32_t  &iLength, uint32_t uiDelayTime);

    virtual int32_t AsynWR(char *sendBuf,int len,const ResponseCallBack &cb,
                           int readLenMax,int timeout);

    virtual int32_t Reset();

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
    bool m_bLogFlag;
    itas109::CSerialPort  * m_SerialPort;
    std::string m_strPort;
    int32_t m_serialPollingTime;
    BaudRate m_baudRate;
    DataBits m_dataBits;
    Parity m_parity;
    StopBits m_stopBits;
    FlowControl m_flowControl;
    std::string m_sPar;

    bool m_bReady = false;

    //接收缓存
    char m_cacheBuff[CACHE_BUFF_MAX] = {0};
    int32_t m_cacheLen = 0;
};

#endif // SERIALPORT_H
