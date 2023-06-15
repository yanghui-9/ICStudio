#include "UpdComm.h"

UpdComm::UpdComm()
{

}

int32_t UpdComm::SendData(char *pBuffer, uint32_t iLength)
{
#if 0
    if(idForServerToClientOrUdpPort == 0 && UdpIp == NULL)
    {
        if(m_udpType == TCPIP_UDP_TYPE_UNICAST)
        {
            if(m_nProperty == TCPIP_TCP_UDP_TYPE_CLIENT)
            {
                io_socklen_t length = sizeof(m_addrin);
                flag = io_sendto(m_UdpSocket,pBuffer,iLength,0,(io_sockaddr *)&m_addrin,length);
            }
            else
            {
                io_socklen_t length = sizeof(m_addrin2);
                flag = io_sendto(m_UdpSocket,pBuffer,iLength,0,(io_sockaddr *)&m_addrin2,length);
            }
        }
        else if(m_udpType == TCPIP_UDP_TYPE_BROADCAST)
        {
            if(m_nProperty == TCPIP_TCP_UDP_TYPE_CLIENT)
            {
                io_socklen_t length = sizeof(m_addrin2);
                flag = io_sendto(m_UdpSocket,pBuffer,iLength,0,(io_sockaddr *)&m_addrin2,length);
            }
            else
            {
                io_socklen_t length = sizeof(m_addrin);
                flag = io_sendto(m_UdpSocket,pBuffer,iLength,0,(io_sockaddr *)&m_addrin,length);
            }
        }
        else
        {
            if(m_nProperty == TCPIP_TCP_UDP_TYPE_CLIENT)
            {
                io_socklen_t length = sizeof(m_addrin2);
                flag = io_sendto(m_UdpSocket,pBuffer,iLength,0,(io_sockaddr *)&m_addrin2,length);
            }
            else
            {
                io_socklen_t length = sizeof(m_addrin);
                flag = io_sendto(m_UdpSocket,pBuffer,iLength,0,(io_sockaddr *)&m_addrin,length);
            }
        }
        if(flag < 0)//错误直接返回-1
            flag  = -1;
    }
    else
    {
        io_sockaddr_in addIn;
        addIn.sin_family = AF_INET;
        addIn.sin_port = htons(idForServerToClientOrUdpPort);
        addIn.sin_addr.s_addr = inet_addr(UdpIp);
        io_socklen_t length = sizeof(addIn);
        flag =  io_sendto(m_UdpSocket,pBuffer,iLength,0,(io_sockaddr *)&addIn,length);
        if(flag < 0)
            flag = -1;
    }
#endif
    return -1;
}

int32_t UpdComm::RecvData(char *pBuffer, int32_t &iLength, uint32_t uiDelayTime)
{
    return -1;
}

int32_t UpdComm::AsynWR(char *sendBuf, int len, const ICommunication::ResponseCallBack &cb, int readLenMax, int timeout)
{
    return -1;
}

int32_t UpdComm::AsynProcess(int32_t iLock)
{
    return -1;
}

int32_t UpdComm::OpenComm(int setnonblock, int Timo,
                          const ConnectCB &connectCB,
                          const DisconnectCB &disconnectCB,
                          const ReadReadyCB &ReadReadyCB)
{
    return 0;
}

int32_t UpdComm::CloseComm()
{
    return -1;
}

int32_t UpdComm::SetCommInfo(const std::string &sPar)
{
    //m_port = static_cast<uint32_t>( stoi(sPar) );
    return -1;
}

int32_t UpdComm::GetCommInfo(std::string &sPar)
{
    //sPar =  std::to_string(m_port);
    return -1;
}




int32_t UpdComm::OpenLog(const char *LogName, int AscOrDec, int OpenType)
{
    return m_log.OpenLog(LogName,AscOrDec,OpenType);
}

int32_t UpdComm::CloseLog()
{
    return m_log.CloseLog();
}

int32_t UpdComm::WriteLog(const char *buf, int len, const char *fmt,...)
{
    int ret;
    io_date_time nowTime = io_getcurDateTime();
    va_list ap;
    va_start(ap, fmt);
    ret = m_log.WriteLog(nowTime.h,nowTime.min,nowTime.s,nowTime.msc,buf,len,fmt,ap);
    va_end(ap);
    return  ret;
}
