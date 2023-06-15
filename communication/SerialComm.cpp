#include "SerialComm.h"

SerialComm::SerialComm(const std::string &sPar)
{
    m_SerialPort = new CSerialPort();
    SetCommInfo(sPar);
    m_SerialPort->setPortName(m_strPort);
}

SerialComm::~SerialComm()
{
    if(m_SerialPort)
    {
        m_SerialPort->close();
        delete  m_SerialPort;
        m_SerialPort = nullptr;
    }
}

int32_t SerialComm::SendData(char *pBuffer, uint32_t iLength)
{
    int flag = 0;
    if(!m_SerialPort->isOpened())
    {
        if(0 != OpenComm())
        {
            m_bReady = false;
            flag = -1;
        }
        else
        {
            m_bReady = true;
        }
    }
    if(flag == 0)
    {
        int iNowWrite  = m_SerialPort->writeData(pBuffer,static_cast<int32_t>( iLength ));
        if(m_bLogFlag)/////日志
        {
            WriteLog(pBuffer,iNowWrite,"【send】send= %d[",iNowWrite);
        }
        if(iNowWrite < 0)
            flag = -1;
        else
            flag = iNowWrite;
    }
    return flag;
}

int32_t SerialComm::RecvData(char *pBuffer, int32_t &iLength, uint32_t uiDelayTime)
{
    int flag = 0;
    io_date_time oldTime;
    io_date_time nowTime = io_getcurDateTime();
    int countRecv = 0;
    int readLen = 0;
    if(!m_SerialPort->isOpened())
    {
        if(0 != OpenComm())
        {
            m_bReady = false;
            WriteLog(nullptr,0,"Serial Port Open Failed Error %ld", m_SerialPort->getLastError());
            flag =  -1;
        }
        else
        {
            m_bReady = true;
        }
    }

    //先读取应用缓存中数据
    if(m_cacheLen > 0)
    {
        if(iLength < m_cacheLen)
        {
            memcpy(pBuffer,m_cacheBuff,static_cast<uint32_t>(iLength) );
            readLen = iLength;
            m_cacheLen -= iLength;
            iLength = m_cacheLen;
            flag = readLen;
        }
        else
        {
           memcpy(pBuffer,m_cacheBuff,static_cast<uint32_t>(m_cacheLen) );
           readLen += m_cacheLen;
           iLength -= m_cacheLen;
           m_cacheLen = 0;
        }
    }

    while(1 && 0 == flag )
    {
        //同步模式下一次最大接收1024字节.
        char buf[1024] = {0};
        int ilen = m_SerialPort->readData(buf,1024);
        if(ilen > 0)
        {
            countRecv++;
            if(m_bLogFlag)/////日志.
            {
                WriteLog(buf,ilen,"【read】recvall-%d-max=%d,read = %d[",countRecv, iLength ,ilen);
            }
            if(readLen + ilen >= iLength)//数据已经超过指定接收长度.
            {
                memcpy(pBuffer+readLen,buf,static_cast<uint32_t>( iLength -readLen ));
                if( (readLen + ilen > iLength)  && (m_cacheLen + (readLen+ilen-iLength) ) <= CACHE_BUFF_MAX)
                {//缓存存的下，超过缓存就丢掉.
                    memcpy(m_cacheBuff + m_cacheLen,buf+(iLength -readLen),static_cast<uint32_t>(readLen+ilen-iLength) );
                    m_cacheLen += readLen+ilen-iLength;
                    iLength = m_cacheLen;//缓存长度返回，待接收字节数.
                }
                readLen += (iLength -readLen );
                break;
            }
            else
            {
                memcpy(pBuffer+readLen, buf, static_cast<uint32_t>(ilen) );
                readLen += ilen;
            }
        }
        //计算时间是否超时.
        oldTime = io_getcurDateTime();
        if(io_getIntervalMsec(oldTime , nowTime) > uiDelayTime)
        {
            break;
        }
        LogService::win_unix_sleep( static_cast<uint32_t>( m_serialPollingTime ));
    }
    if(flag == 0) flag = readLen;
    return flag;
}

int32_t SerialComm::AsynWR(char *sendBuf, int len, const ICommunication::ResponseCallBack &cb, int readLenMax, int timeout)
{
    return -1;
}

int32_t SerialComm::Reset()
{
    return -1;
}

int32_t SerialComm::AsynProcess(int32_t iLock)
{
    return -1;
}

int32_t SerialComm::OpenComm(int setnonblock, int Timo,
                             const ConnectCB &connectCB,
                             const DisconnectCB &disconnectCB,
                             const ReadReadyCB &ReadReadyCB)
{
    Q_UNUSED(setnonblock);
    Q_UNUSED(Timo);

    int flag = 0;
    if(!m_bReady)
    {
        m_SerialPort->setOperateMode(itas109::SynchronousOperate);//设置成同步操作模式
        //此函数不加锁，提供类内部调用
        if(m_SerialPort->isOpened())
            m_SerialPort->close();
        if(m_SerialPort->open())
        {
            m_SerialPort->setFlowControl(m_flowControl);
            m_SerialPort->setReadTimeInterval(m_serialPollingTime);
            m_SerialPort->setBaudRate(m_baudRate);
            m_SerialPort->setDataBits(m_dataBits);
            m_SerialPort->setParity(m_parity);
            m_SerialPort->setStopBits(m_stopBits);
            flag = 0;
            m_bReady = true;
        }
        else
        {
            m_bReady = false;
            flag =  1;
        }
    }
    return flag;
}

int32_t SerialComm::CloseComm()
{
    if(m_SerialPort)
    {
        m_SerialPort->close();
        m_bReady = false;
    }
    return -1;
}

int32_t SerialComm::SetCommInfo(const std::string &sPar)
{
    //m_port = static_cast<uint32_t>( stoi(sPar) );
    m_sPar = sPar;
    std::vector<std::string> sVector = LogService::StringSplit(sPar,"-");
    if(7 == sVector.size())
    {
        //1
        m_strPort = sVector.at(0);
        //2
       m_baudRate = static_cast<BaudRate>(stoi(sVector.at(1)));
       //3
       m_dataBits = static_cast<DataBits>( stoi(sVector.at(2)) );
       //4
       switch (sVector.at(3).at(0)) {
       case 'N':
       {
           m_parity = ParityNone;
       }
           break;
       case 'O':
       {
           m_parity = ParityOdd;
       }
           break;
       case 'E':
       {
           m_parity = ParityEven;
       }
           break;
       case 'M':
       {
           m_parity = ParityMark;
       }
           break;
       case 'S':
       {
           m_parity = ParitySpace;
       }
           break;
       default:
       {
           m_parity = ParityNone;
       }
       }

       //5
       if("1" == sVector.at(4))
       {
           m_stopBits = StopOne;
       }
       else if ("1.5" == sVector.at(4)) {
           m_stopBits = StopOneAndHalf;
       }
       else {
           m_stopBits = StopTwo;
       }

       //6
       m_serialPollingTime = stoi(sVector.at(5));

       //7
       m_flowControl = static_cast<FlowControl>(stoi(sVector.at(6)));

    }
    return -1;
}

int32_t SerialComm::GetCommInfo(std::string &sPar)
{
    sPar =  m_sPar;
    return -1;
}




int32_t SerialComm::OpenLog(const char *LogName, int AscOrDec, int OpenType)
{
    m_bLogFlag = true;
    return m_log.OpenLog(LogName,AscOrDec,OpenType);
}

int32_t SerialComm::CloseLog()
{
    m_bLogFlag = false;
    return m_log.CloseLog();
}

int32_t SerialComm::WriteLog(const char *buf, int len, const char *fmt,...)
{
    int ret;
    io_date_time nowTime = io_getcurDateTime();
    va_list ap;
    va_start(ap, fmt);
    ret = m_log.WriteLog(nowTime.h,nowTime.min,nowTime.s,nowTime.msc,buf,len,fmt,ap);
    va_end(ap);
    return  ret;
}
