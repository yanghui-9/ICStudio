#include "TcpClient.h"
#include <mutex>
#include <QDebug>

TcpClient::TcpClient(const std::string &sPar)
{
   SOCK_STARTUP();
   SetCommInfo(sPar);
}

TcpClient::~TcpClient()
{
    m_monitorThreadStopFlag = false;
    if(m_monitorThread)
        m_monitorThread->join();
}

int32_t TcpClient::SendData(char *pBuffer, uint32_t iLength)
{
    //io_lock(&m_commLock);
    static int flag = 0;
    int len= 0;
    len = io_write(m_ClientSocket,pBuffer,iLength);
    if(len >= 0)
    {
        flag =  len;
    }
    else if(socket_errno != EINPROGRESS && socket_errno != EWOULDBLOCK)
    {//确实是通讯错误需要重连的
        if(m_bLogFlag)
        {
            WriteLog(pBuffer,0,"client,sendError= %d[", len);
        }
        flag = -1;
    }
    else
    {
        flag = 0;
    }
    if(m_bLogFlag)
    {
        WriteLog(pBuffer,len,"client,%d-send = %d[",iLength,len);
    }
    //io_unlock(&m_commLock);
    return flag;
}

int32_t TcpClient::RecvData(char *pBuffer, int32_t &iLength, uint32_t uiDelayTime)
{
    int flag = -1;
    int nReturn = 0;
    int tv_sec = uiDelayTime/1000;
    int tv_usec = (uiDelayTime*1000)%1000000;
    struct timeval tv = {tv_sec, tv_usec};

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(m_ClientSocket, &fds);  // wait for response
#ifdef WIN32
    nReturn = select(0, &fds, nullptr, nullptr, &tv);
#else
    nReturn = select(m_ClientSocket +1, &fds,NULL,NULL,&tv);
#endif

    if (nReturn == 0)
    {
        if(m_bLogFlag)
        {
            WriteLog(nullptr,0,"【read】client,read-timeout = %d[", nReturn);
        }
        flag = 0;
    }
    else
    {
        flag = 0;
        int iRecv = io_read(m_ClientSocket, pBuffer, static_cast<uint32_t>( iLength ));
        if(iRecv >= 0)
        {
            if(m_bLogFlag)
            {
                WriteLog(pBuffer,iRecv,"【read】client,read = %d[", iRecv);
            }
            flag += iRecv;
            qDebug()<<__func__<<iRecv<<socket_errno;
        }
        else
        {
            if(socket_errno != EINPROGRESS && socket_errno != EWOULDBLOCK)
            {
                if(m_bLogFlag)
                {
                    WriteLog(pBuffer,0,"【read】client,readError = %d[", iRecv);
                }
               flag = -1;
            }
            flag = 0;
        }
    }
    return flag;
}

int32_t TcpClient::AsynWR(char *sendBuf, int len, const ICommunication::ResponseCallBack &cb, int readLenMax, int timeout)
{
    //检查异步线程.
    AsynRun();

    //入队.
    if(AsynWR_Max > m_aysnRequestQueue.size())
    {//避免任务队列无限制增加.
        AsynRequest request;
        request.timeout = timeout;
        request.maxLen = readLenMax;
        request.CB = cb;
        memcpy(request.Buff,sendBuf,static_cast<uint>(len));
        request.sendLen = len;
        m_aysnRequestQueue.enqueue(request);

        //唤醒工作线程
        {
            //加锁
            std::unique_lock<std::mutex> lk(cvMutex);
            //唤醒
            cv.notify_all();
        }
    }

    return 0;
}

void TcpClient::MonitorThread()
{
    isRun = true;
    while(m_monitorThreadStopFlag)
    {
        //条件判断.
        {
            //加锁.
            std::unique_lock<std::mutex> lk(cvMutex);
            //检测队列是否存在任务，不存在就阻塞.
            cv.wait(lk, [this] { return !this->m_aysnRequestQueue.empty();});
        }

        //开始处理任务.
        AsynRequest Request;
        if(m_aysnRequestQueue.dequeue(Request))
        {
            int iSend = SendData(Request.Buff,static_cast<uint32_t>(Request.sendLen));
            if(Request.sendLen == iSend)
            {
                int iRecv =  RecvData(Request.Buff,Request.maxLen,static_cast<uint32_t>( Request.timeout ));
                //回调.
                if(-1  == iRecv){
                    Request.CB(-1,Request.Buff,0);
                }
                else {
                    Request.CB(0,Request.Buff,iRecv);
                }
            }
            else
            {//发送异常.
                Request.CB(-1,Request.Buff,0);
            }
        }
    }

    isRun = false;
}

int32_t TcpClient::AsynProcess(int32_t iLock)
{
    return -1;
}

int32_t TcpClient::OpenComm(int setnonblock, int Timo,
                            const ConnectCB &connectCB,
                            const DisconnectCB &disconnectCB,
                            const ReadReadyCB &ReadReadyCB)
{
    if(0 != m_ClientSocket)
    {//已经连接了.
        return 0;
    }

    int ret = 0;
    m_ClientSocket =  io_socket_stream();
    io_set_sock_recv_timeo(m_ClientSocket, 500);
    io_set_sock_snd_timeo(m_ClientSocket, 500);
    if(1 == setnonblock)//非阻塞，使用线程来连接.
    {
        m_arg.m_ip = m_sIp;
        m_arg.m_port = m_iPort;
        m_arg.sock = m_ClientSocket;
        io_set_nonblock(m_ClientSocket);
        //io_thread_create(&m_connect,io_connect_thread,(void *)(&m_arg),0);
    }
    else //阻塞，超时模式.
    {
        io_sockaddr_in    addr;
        SOCKADDR_IN(addr, m_sIp.data(),m_iPort);
        if(-1 == Timo )
            Timo = m_connectTimo;
        else
            m_connectTimo = Timo;
        int iSuc = io_connect_timeo(m_ClientSocket,&addr,Timo);
        io_set_nonblock(m_ClientSocket);
        if(0 == iSuc)
        {
            if(m_bLogFlag)
            {
                WriteLog(nullptr,0,"connect success\n");
            }
        }
        else
        {
            if(m_bLogFlag)
            {
                WriteLog(nullptr,0,"connect fail error = %d\n",socket_errno);
            }
            CloseComm();
            ret = 1;
        }
    }
    return ret;
}

int32_t TcpClient::CloseComm()
{
    int ret = io_close_handle(m_ClientSocket);
    m_ClientSocket = 0;
    return ret;
}

int32_t TcpClient::SetCommInfo(const std::string &sPar)
{
    std::vector<std::string> sVector = LogService::StringSplit(sPar,"-");
    if(2 == sVector.size())
    {
        m_sIp = sVector.at(0);
        m_iPort = std::stoi(sVector.at(1));
        m_sPar = sPar;
    }
    return 0;
}

int32_t TcpClient::GetCommInfo(std::string &sPar)
{
    sPar = m_sPar;
    return 0;
}

int32_t TcpClient::OpenLog(const char *LogName, int AscOrDec, int OpenType)
{
    return m_log.OpenLog(LogName,AscOrDec,OpenType);
}

int32_t TcpClient::CloseLog()
{
    return m_log.CloseLog();
}

int32_t TcpClient::WriteLog(const char *buf, int len, const char *fmt,...)
{
    int ret;
    io_date_time nowTime = io_getcurDateTime();
    va_list ap;
    va_start(ap, fmt);
    ret = m_log.WriteLog(nowTime.h,nowTime.min,nowTime.s,nowTime.msc,buf,len,fmt,ap);
    va_end(ap);
    return  ret;
}
