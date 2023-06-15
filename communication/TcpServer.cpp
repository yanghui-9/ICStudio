#include <qglobal.h>
#include "TcpServer.h"
#include "socket/io_select.h"


//处理客户端连接事件回调函数.
int TcpServer::_io_tcp_conn_cb(io_t &iop, unsigned int events)
{
    int n = 0;
    int r = 0;

    if(events & EV_TYPE_DESTROY)
    {//断开.
        _io_remove_client(m_base,iop);
        return -1;
    }

    if(EV_IS_SET_R(events))
    {//读.
        char buf[2048] = {0};
        n = io_read(iop.handle,buf,2048);
        if(n < 0 )
        {
            if(socket_errno != EINPROGRESS && socket_errno != EWOULDBLOCK)
            {
                /*socket error*/
                _io_remove_client(m_base,iop);
                return -1;
            }
        }
        if(n > 0)
        {
            do
            {
                uint64_t oldLen = iop.rbuf.size();
                uint64_t newLen = oldLen + static_cast<uint>(n);
                if(newLen > m_base.maxbuf)
                {//超过接收缓存.
                    break;
                }
                iop.rbuf.resize(newLen);
                memcpy(iop.rbuf.data()+ oldLen,buf,static_cast<uint>(n));
                r = io_read(iop.handle,buf,2048);
                if(r < 0)
                {
                    if(socket_errno != EINPROGRESS && socket_errno != EWOULDBLOCK)
                    {
                        /*socket error*/
                        _io_remove_client(m_base,iop);
                        return -1;
                    }
                }
                else {
                  n += r;
                }
            }while(r > 0);
        }
        if(0 < n)
        {//处理接收数据.
            if(m_ReadReadyCB)
            {
               uint32_t rlen = static_cast<uint32_t>( iop.rbuf.size() );
               uint32_t sLen = 0;
               char * sBuff = nullptr;
               int len = m_ReadReadyCB( 0,iop.rbuf.data(),rlen,&sBuff,&sLen);
               if(len > 0)
               {
                   if(static_cast<uint32_t>(len) <= rlen )
                   {
                       memmove(iop.rbuf.data(),iop.rbuf.data()+n,rlen - static_cast<uint32_t>(n));
                       iop.rbuf.resize(rlen - static_cast<uint32_t>(n));
                   }
                   else
                   {
                       iop.rbuf.clear();
                   }
               }
               //处理发送.
               if(0 < sLen)
               {
                   n = io_write(iop.handle,sBuff,sLen);
                   if(0 > n)
                   {
                       if(socket_errno != EINPROGRESS && socket_errno != EWOULDBLOCK)
                       {
                           /*socket error*/
                           _io_remove_client(m_base,iop);
                           return -1;
                       }
                   }
                   else if( sLen > static_cast<uint32_t>(n) &&
                            (sLen - static_cast<uint32_t>(n) + iop.sbuf.size() ) <= m_base.maxbuf )
                   {//没发完，存起来.
                       iop.sbuf.resize(sLen - static_cast<uint32_t>(n) + iop.sbuf.size());
                       memcpy(iop.sbuf.data(),sBuff+static_cast<uint32_t>(n),sLen - static_cast<uint32_t>(n));
                   }
               }
            }
        }
    }

    if(EV_IS_SET_W(events))
    {//可写且写入缓存有数据.
        if(0 < iop.sbuf.size())
        {
           uint32_t slen = static_cast<uint32_t>( iop.sbuf.size() );
           n = io_write(iop.handle,iop.sbuf.data(),slen);
           if(n < 0)
           {//连接断开.
               if(socket_errno != EINPROGRESS && socket_errno != EWOULDBLOCK)
               {
                   /*socket error*/
                   _io_remove_client(m_base,iop);
                   return -1;
               }
           }
           else if(static_cast<uint32_t>(n) <= slen )
           {
              memmove(iop.sbuf.data(),iop.sbuf.data()+n,slen - static_cast<uint32_t>(n));
              iop.sbuf.resize(slen - static_cast<uint32_t>(n));
           }
           else
           {
               iop.sbuf.clear();
           }
        }
    }

    if(EV_IS_SET(events,EV_TYPE_TIMEOUT))
    {//客户端活跃超时.
        /*keepalive timeout error*/
        _io_remove_client(m_base,iop);
    }
    return 0;
}

//服务器iop事件回调函数.
int TcpServer::_io_tcp_server_cb(unsigned int events)
{
    if(events & EV_TYPE_READ)
    {
        io_handle_t h = io_accept(m_base.handle,nullptr,nullptr);
        if(h == INVALID_HANDLE)
        {
            return 0;
        }
        io_set_nonblock(h);
        io_t iop;
        iop.handle = h;
        iop.timeout = 30;
        iop.port = io_getsockport(h);
        iop.sIp = std::string(io_getsockip(h));
        iop.last_dispatch_time = m_base.cur_time;
        //添加到事件模型中.
        int r = (*(m_base.op_imp._base_add))(&m_base, 0, h, events);
        if(r != 0)
        {
            _io_remove_client(m_base,iop);
            return -1;
        }
        m_base.iops.push_back(iop);

        //处理客户端连接.
        if(m_connectCB)
        {
           m_connectCB(iop.sIp,iop.port);
        }
    }
    return 0;
}

//删除客户端.
void TcpServer::_io_remove_client(io_base_t &base, io_t &iop)
{
    //处理删除客户端回调.
    if(m_disconnectCB)
    {
        m_disconnectCB(iop.sIp,iop.port);
    }

    //删除.
    if(iop.handle != INVALID_HANDLE)
    {
        //从事件模型中删除该对象.
        if(base.op_imp._base_del)
            (*(base.op_imp._base_del))(&base, 0, iop.handle);
        else
        {
            printf("io_del error\n");
        }
        io_close_handle(iop.handle);
        iop.handle = INVALID_HANDLE;
    }
    base.iops.remove(iop);
}


TcpServer::TcpServer(const std::string &sPar)
{
    m_sPar = sPar;
    SetCommInfo(sPar);
}

TcpServer::~TcpServer()
{
    //退出事件线程.
    m_ThreadStopFlag = false;
    if(m_Thread)
        m_Thread->join();

    //释放服务端socket.
    if(m_base.handle != INVALID_HANDLE)
    {
        //从事件模型中删除该对象.
        if(m_base.op_imp._base_del)
            (*(m_base.op_imp._base_del))(&m_base, 0, m_base.handle);
        else
        {
            printf("io_del error\n");
        }
        io_close_handle(m_base.handle);
        m_base.handle = INVALID_HANDLE;
    }

    //释放当前客户端连接.
    foreach (auto iop, m_base.iops) {
        _io_remove_client(m_base,iop);
    }

    //清除事件模型数据.
    if(m_base.op_imp._base_free)
    {
        (*((m_base.op_imp)._base_free))(&m_base);
    }
}

int32_t TcpServer::SendData(char *pBuffer, uint32_t iLength)
{
    return -1;
}

int32_t TcpServer::RecvData(char *pBuffer, int32_t &iLength, uint32_t uiDelayTime)
{
    return -1;
}

int32_t TcpServer::AsynWR(char *sendBuf, int len, const ICommunication::ResponseCallBack &cb, int readLenMax, int timeout)
{
    return -1;
}

int32_t TcpServer::AsynProcess(int32_t iLock)
{
    int r = 0;

    //事件查询调度.
    base_dispatch dispatch_cb = m_base.op_imp._base_dispatch;
    if(!dispatch_cb)
    {
        printf("io_dispatch error \n");
        return -1;
    }
    r = (*dispatch_cb)(&m_base,m_base.dispatch_interval);
    if( r == -1)
    {
        return -1;
    }

    //检查客户端活跃.
    if(m_base.cur_time > m_base.last_time)
    {
        /*********clear keepalive, 60 seconds per times***********************/
        /********* keepaliveto 30 seconds per times***********************/
        if(m_base.cur_time > m_base.last_keepalive_time + 30)
        {
            m_base.last_keepalive_time = m_base.cur_time;
            foreach (auto iop, m_base.iops) {
                if(iop.timeout > 0 && iop.last_dispatch_time + iop.timeout < m_base.cur_time)
                {
                    m_base.clientEventCB(iop,EV_TYPE_TIMEOUT);
                }
            }
        }
        m_base.last_time = m_base.cur_time;
    }

    return r;
}

int32_t TcpServer::OpenComm(int setnonblock, int Timo,
                            const ConnectCB &connectCB,
                            const DisconnectCB &disconnectCB,
                            const ReadReadyCB &ReadReadyCB)
{
    m_connectCB = connectCB;
    m_disconnectCB = disconnectCB;
    m_ReadReadyCB = ReadReadyCB;

    if(-1 == Init())
    {//打开失败错误码
        return 1;
    }

    return 0;
}

int32_t TcpServer::CloseComm()
{
    return -1;
}

int32_t TcpServer::SetCommInfo(const std::string &sPar)
{
    m_port = static_cast<uint32_t>( stoi(sPar) );
    return -1;
}

int32_t TcpServer::GetCommInfo(std::string &sPar)
{
    sPar =  std::to_string(m_port);
    return -1;
}

int32_t TcpServer::OpenLog(const char *LogName, int AscOrDec, int OpenType)
{
    return m_log.OpenLog(LogName,AscOrDec,OpenType);
}

int32_t TcpServer::CloseLog()
{
    return m_log.CloseLog();
}

int32_t TcpServer::WriteLog(const char *buf, int len, const char *fmt,...)
{
    int ret;
    io_date_time nowTime = io_getcurDateTime();
    va_list ap;
    va_start(ap, fmt);
    ret = m_log.WriteLog(nowTime.h,nowTime.min,nowTime.s,nowTime.msc,buf,len,fmt,ap);
    va_end(ap);
    return  ret;
}

int32_t TcpServer::Init()
{
    if(!m_bInitFlag)
    {
        //初始化事件模型.
        io_init_select(reinterpret_cast<void *>(&m_base),0);

        //服务器socket初始化.
        m_base.handle = INVALID_HANDLE;
        m_base.handle = io_tcp_server("127.0.0.1",static_cast<ushort>( m_port ));
        if(INVALID_HANDLE == m_base.handle){
            return -1;
        }

        //添加到事件模型中.
        int r = (*(m_base.op_imp._base_add))(&m_base, 0, m_base.handle, EV_TYPE_READ);
        if(r < 0 )
        {
            return -1;
        }

        //设置回调.
        m_base.clientEventCB = std::bind(&TcpServer::_io_tcp_conn_cb,this,std::placeholders::_1,std::placeholders::_2);
        m_base.serverEventCB = std::bind(&TcpServer::_io_tcp_server_cb,this,std::placeholders::_1);

        //初始化参数.
        m_base.maxio = 1024;			//最大并发数.
        m_base.maxbuf = 64*1024*1024;	//单个发送或接收缓存的最大值.
        m_base.dispatch_interval = DEFAULT_DISPATCH_INTERVAL;
        m_base.cur_time = time(nullptr);
        m_base.last_time = m_base.cur_time;
        m_base.last_keepalive_time = m_base.cur_time;

        m_bInitFlag = true;

        //启动事件线程.
        if(!m_Thread)
        {
            m_Thread = std::make_shared<std::thread>(&TcpServer::RUN,this);
        }
    }

    return 0;
}
