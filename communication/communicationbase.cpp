#include "communicationbase.h"

communicationbase::communicationbase()
{
    io_lock_init(&m_logLock);
    m_io_log_service = 0;
    m_logFlag = 0;
}

communicationbase::~communicationbase()
{
    io_lock_destroy(&m_logLock);
    CloseLog();
}

int communicationbase::LOG_DBG2(io_log_service_t *logObj, int q_hour, int q_min, int q_sec, int q_msec, const char *buf1, int len1, const char *fmt,...)
{
    io_lock(&m_logLock);
    va_list ap;
    va_start(ap, fmt);
    ::LOG_DBG2(logObj,q_hour,q_min,q_sec,q_msec,buf1,len1,fmt,ap);
    va_end(ap);
    io_unlock(&m_logLock);
    return 0;
}

void communicationbase::win_unix_sleep(unsigned long sec)
{
#if defined(Q_OS_WIN)
    ::Sleep(sec);
#else
    usleep(sec*1000);
#endif
}

int32_t communicationbase::OpenLog(const char *LogName, int AscOrDec, int OpenType)
{
    if( m_logFlag == 0)
    {
        io_lock(&m_logLock);
        m_logFlag = 1;
        m_io_log_service = (io_log_service_t *)malloc(sizeof(io_log_service_t));
        int flag =  io_log_service_start(m_io_log_service,"",LogName,100,AscOrDec,OpenType);
        io_unlock(&m_logLock);
        return flag;
    }
    else
        return -1;
}

int32_t communicationbase::CloseLog()
{
    if( m_logFlag == 1)
    {
        io_lock(&m_logLock);
        m_logFlag  = 0;
        int flag = io_log_service_stop(m_io_log_service);
        m_io_log_service = 0;
        io_unlock(&m_logLock);
        return flag;
    }
    else
    {
        return -1;
    }
}

int32_t communicationbase::WriteLog(const char *log)
{
    if(1 == m_logFlag)
    {
        //QTime nowTime = QTime::currentTime();
        //char *buf =0;
        //LOG_DBG2(m_io_log_service,nowTime.hour(),nowTime.minute(),nowTime.second(),nowTime.msec(),buf,0,"%s[",log);
    }
    return 0;
}

