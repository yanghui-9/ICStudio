#include "LogService.h"

LogService::LogService()
{

}

int32_t LogService::OpenLog(const char *LogName, int AscOrDec, int OpenType)
{
    if( m_logFlag == 0)
    {
        //io_lock(&m_logLock);
        m_logFlag = 1;
        m_io_log_service = static_cast<io_log_service_t *>( malloc(sizeof(io_log_service_t)) );
        int flag =  io_log_service_start(m_io_log_service,"",LogName,100,AscOrDec,OpenType);
        //io_unlock(&m_logLock);
        return flag;
    }
    return 0;
}

int32_t LogService::CloseLog()
{
    if( m_logFlag == 1)
    {
        //io_lock(&m_logLock);
        m_logFlag  = 0;
        int flag = io_log_service_stop(m_io_log_service);
        m_io_log_service = nullptr;
        //io_unlock(&m_logLock);
        return flag;
    }
    return 0;
}

int32_t LogService::WriteLog(int q_hour, int q_min, int q_sec, int q_msec,
                             const char *buf1, int len1, const char *fmt,va_list &ap)
{
    if(1 == m_logFlag && m_io_log_service)
    {
        //io_lock(&m_logLock);
        ::LOG_DBG2(m_io_log_service,q_hour,q_min,q_sec,q_msec,buf1,len1,fmt,ap);
        //io_unlock(&m_logLock);
        return 0;
    }
    return 0;
}

//字符串分割函数.
std::vector<std::string> LogService::StringSplit(const std::string &str,const std::string &pattern)
{
    std::vector<std::string> resVec;
    if ("" == str)
    {
        return resVec;
    }
    //方便截取最后一段数据.
    std::string strs = str + pattern;

    size_t pos = strs.find(pattern);
    size_t size = strs.size();
    while (pos != std::string::npos)
    {
        std::string x = strs.substr(0,pos);
        resVec.push_back(x);
        strs = strs.substr(pos+1,size);
        pos = strs.find(pattern);
    }
    return resVec;
}
void LogService::win_unix_sleep(unsigned long sec)
{
#if defined(Q_OS_WIN)
    ::Sleep(sec);
#else
    usleep(sec*1000);
#endif
}

