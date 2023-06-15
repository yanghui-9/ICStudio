#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "communicationdef.h"
#include "LogService.h"
#include <list>
#include "socket/io_def.h"


class TcpServer : public ICommunication
{
public:
    TcpServer(const std::string &sPar);
    virtual ~TcpServer();

    virtual int32_t SendData(char * pBuffer,  uint32_t iLength);

    virtual int32_t RecvData(char * pBuffer,  int32_t &iLength, uint32_t uiDelayTime);

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


    //日志.
    virtual int32_t OpenLog(const char * LogName,int AscOrDec =0,int OpenType = 2);

    virtual int32_t CloseLog();

    int32_t WriteLog(const char *buf, int len, const char *fmt,...);


    //客户端事件回调.
    int _io_tcp_conn_cb(io_t &iop,unsigned int events);
    //服务端事件回调.
    int _io_tcp_server_cb(unsigned int events);
    //断开客户端.
    void _io_remove_client(io_base_t &base,io_t &iop);

private:
    //初始化服务器.
    int32_t Init();

    void RUN()
    {
        while (m_ThreadStopFlag) {
          AsynProcess();
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

private:
    LogService m_log;
    uint32_t m_port = 8888;
    io_base_t m_base;
    bool m_bInitFlag = false;

    ConnectCB m_connectCB = nullptr;
    DisconnectCB m_disconnectCB = nullptr;
    ReadReadyCB m_ReadReadyCB = nullptr;

    std::string m_sPar;

    std::shared_ptr<std::thread> m_Thread;
    bool m_ThreadStopFlag = true;
};

#endif // TCPSERVER_H
