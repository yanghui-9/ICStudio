/*
 * Copyright (c) 2011-2014 zhangjianlin 张荐林
 * eamil:jonas88@sina.com
 * addr: china
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _IOP_LOG_SERVICE_H_
#define _IOP_LOG_SERVICE_H_

#include <stdio.h>

#include "io_config.h"
//#include "io_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

//日志级别，默认支持  错误，信息，调试 三类
#define IOP_LOG_ALERT 9
#define IOP_LOG_ERR 10
#define IOP_LOG_INFO 11
#define IOP_LOG_DBG  12
#define IOP_LOG_ALL 100         //所有级别
#define IOP_LOG_NONE 0          //不写任何日志

#define IOP_MAX_LOG_PATH 256
//#define IOP_MAX_LOG_BUF_SIZE 1048576
struct tag_io_log_service
{
    //io_lock_t lock;
    char path[IOP_MAX_LOG_PATH+1];
    char prefix[IOP_MAX_LOG_PATH+1];
    FILE *fp;
    char log_level;
    int m_day;
    //io_thread_t tid;
    //char log_buf[IOP_MAX_LOG_BUF_SIZE];
    //int log_buf_size;
    //volatile int stop_flag;
    int AscOrDec;
    int type;//0-w,1-w+,2-a,3-a+
};
typedef struct tag_io_log_service io_log_service_t;
extern io_log_service_t *g_io_log_service;

//int io_log_service_start(const char *path, const char *prefix, char level, int AscOrDec);
int io_log_service_start(io_log_service_t  *logObj, const char *path, const char *prefix, char level, int AscOrDec, int type);
int io_log_service_stop(io_log_service_t *logObj);
//int io_log_flush(io_log_service_t *log);

//int io_log_bin(io_log_service_t *logObj,int level, const void *buf, unsigned int len);
int io_log_fmt(io_log_service_t *logObj,void *buf, int len, int level, const char *fmt,...);

int LOG_DBG(io_log_service_t *logObj,const char *fmt,...);
int LOG_DBG1(io_log_service_t *logObj,const char *buf1, int len1);
int LOG_DBG2(io_log_service_t *logObj,int q_hour, int q_min, int q_sec, int q_msec, const char *buf1, int len1, const char *fmt,va_list ap);
int LOG_INF(io_log_service_t *logObj,const char *fmt,...);
int LOG_ERR(io_log_service_t *logObj,const char *fmt,...);
int LOG_ALT(io_log_service_t *logObj,const char *fmt,...);



#ifdef __cplusplus
}
#endif 		//__Cplusplus

#endif		

