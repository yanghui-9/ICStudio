#ifndef LOGSERVICE_H
#define LOGSERVICE_H
#include <stdint.h>
#include <qglobal.h>
#include <list>
#include <vector>
#include <string>
#include "socket/io_log_service.h"
#include "socket/io_util.h"

class LogService
{
public:
    LogService();

    int32_t OpenLog(const char * LogName,int AscOrDec =0,int OpenType = 2);
    int32_t CloseLog();
    int32_t WriteLog(int q_hour, int q_min, int q_sec, int q_msec,
                     const char *buf1, int len1, const char *fmt, va_list &ap);

    //工具方法
    static void win_unix_sleep(unsigned long sec);
    static std::vector<std::string> StringSplit(const std::string &str,const std::string &pattern);


private:
    io_log_service_t *m_io_log_service;
    int m_logFlag = 0;
};

#endif // LOGSERVICE_H
