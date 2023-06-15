#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <thread>
#include "communicationdef.h"
#include "LogService.h"
#include "SafeQueue.h"
#define READ_BUFF_MAX (4096)

typedef struct
{
    io_handle_t sock;
    std::string m_ip;
    int  m_port;
}connectArg;

typedef struct
{
    int sendLen = 0;
    char Buff[READ_BUFF_MAX] = {0};
    ICommunication::ResponseCallBack CB;
    int timeout = 100;
    int maxLen = 0;
}AsynRequest;

#define AsynWR_Max  (1024)

class TcpClient : public ICommunication
{
public:
    TcpClient(const std::string &sPar);
    virtual ~TcpClient();

    virtual int32_t SendData(char * pBuffer,  uint32_t iLength);

    virtual int32_t RecvData(char * pBuffer,  int32_t  &iLength,uint32_t uiDelayTime);

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


    //异步线程.
    void AsynRun(){
      if(!m_monitorThread)
      {
          m_monitorThread = std::make_shared<std::thread>(&TcpClient::MonitorThread,this);
      }
    }

    //监控线程.
    void MonitorThread();

    //日志.
    virtual int32_t OpenLog(const char * LogName,int AscOrDec =0,int OpenType = 2);

    virtual int32_t CloseLog();

    int32_t WriteLog(const char *buf, int len, const char *fmt,...);

private:
    LogService m_log;
    bool m_bLogFlag = false;
    io_handle_t m_ClientSocket = 0;
    connectArg m_arg;
    std::string m_sPar;
    std::string m_sIp;
    int32_t m_iPort;
    int32_t m_connectTimo;

    //异步线程
    std::shared_ptr<std::thread> m_monitorThread;
    bool isRun = false;
    bool m_monitorThreadStopFlag = true;
    SafeQueue<AsynRequest> m_aysnRequestQueue;
    //条件变量控制
    std::mutex cvMutex;
    std::condition_variable cv;
};

#endif // TCPCLIENT_H
