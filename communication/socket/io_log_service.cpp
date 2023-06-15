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
#include <time.h>
#include <stdarg.h>
/*测试用，yanghui，0831*/
//#include <QTime>
/*测试用，yanghui，0831*/

#include "io_log_service.h"

io_log_service_t *g_io_log_service = (io_log_service_t *)0;
int AscToDec(char * buf,int len,int totalLen)
{
    int len1 = 3*len - 1;
    if(len1>totalLen)
        return -1;
    char buf1[1024]={0};
    char str1[4] ={0};
    int i;
    for( i= 0;i<len;i++)
    {
        sprintf(str1,"%.2X ",(unsigned char)(*(buf+i)));
        buf1[i*3] = str1[0];
        buf1[i*3+1] = str1[1];
        buf1[i*3+2] = 32;
    }
    memcpy(buf,buf1,len1);
    return len1;
}

static int __io_log_reopen(io_log_service_t *log_service)
{
    int r = 0;
    struct tm t;
	time_t n;
    char file[3000];
	FILE *tmp_fp;
    if(strcmp(log_service->path,"stdout/")==0)
    {
        log_service->fp = stdout;
        return 0;
    }
    n = time(NULL);
	t = *localtime(&n);
    if( (log_service->fp) && (t.tm_mday == log_service->m_day))
    {
        return 0;
    }
#ifdef WIN32
   // sprintf(file, sizeof(file)-1,"%s%s%04d%02d%02d.log", log_service->path,log_service->prefix, 1900+t.tm_year, 1+t.tm_mon, t.tm_mday);
    sprintf(file,"%s%s%04d%02d%02d.log", log_service->path,log_service->prefix, 1900+t.tm_year, 1+t.tm_mon, t.tm_mday);
	tmp_fp = 0;
    //fopen_s(&tmp_fp,file, "a");
   switch(log_service->type)
   {
   case 0:
       tmp_fp = fopen(file, "w");
       break;
   case 1:
       tmp_fp = fopen(file, "w+");
       break;
   case 2:
       tmp_fp = fopen(file, "a");
       break;
   case 3:
       tmp_fp = fopen(file, "a+");
       break;
   default:
       tmp_fp = fopen(file, "a");
   }
#else
	snprintf(file, sizeof(file)-1,"%s%s%04d%02d%02d.log", log_service->path,log_service->prefix, 1900+t.tm_year, 1+t.tm_mon, t.tm_mday); 
    switch(log_service->type)
    {
    case 0:
        tmp_fp = fopen(file, "w");
        break;
    case 1:
        tmp_fp = fopen(file, "w+");
        break;
    case 2:
        tmp_fp = fopen(file, "a");
        break;
    case 3:
        tmp_fp = fopen(file, "a+");
        break;
    default:
        tmp_fp = fopen(file, "a");
    }
#endif
    if(!tmp_fp)
    {
        r = -1;
    }
    else
    {
        if(log_service->fp){fclose(log_service->fp);}
        log_service->fp = tmp_fp;
    }
    log_service->m_day = t.tm_mday;
    return 0;
}

int io_log_service_start(io_log_service_t * logObj, const char *path, const char *prefix, char level, int AscOrDec,int type)
{
	io_log_service_t *log;
   // if(logObj)
    //{
    //    return 0;
   // }
    //logObj = (io_log_service_t *)malloc(sizeof(io_log_service_t));
    if(!logObj)
    {
        return -1;
    }
    log = logObj;
    log->log_level = level;
    //log->log_buf_size = 0;
    log->m_day = -1;
    //log->stop_flag = 0;
    log->AscOrDec = AscOrDec;
    log->type = type;
    memset((void *)(log->path), 0, IOP_MAX_LOG_PATH+1);
    memset((void *)(log->prefix), 0, IOP_MAX_LOG_PATH+1);
    
    if(path)
    {
        int path_len = strlen(path);
#ifdef WIN32
        //strcpy_s((char *)(log->path),IOP_MAX_LOG_PATH-1,path);
        strcpy((char *)(log->path),path);
#else
        strncpy((char *)(log->path), path,IOP_MAX_LOG_PATH);

#endif
		if(path_len == 0)
        {
            //log->path[0] = '/';//注释当path=“”时为当前路径
            log->path[1] = 0;
        }
        else
        {
            if((log->path)[path_len-1] != '/')
            {
                log->path[path_len] = '/';
                log->path[path_len+1] = 0;
            }
        }
    }
    else
    {
#ifdef WIN32
        //strcpy_s((char *)(log->path),IOP_MAX_LOG_PATH, "./");
        strcpy((char *)(log->path), "./");

#else
		strncpy((char *)(log->path), "./",IOP_MAX_LOG_PATH);
#endif
	}
    if(prefix)
    {
#ifdef WIN32
       // strcpy_s((char *)(log->prefix), IOP_MAX_LOG_PATH,prefix);
        strcpy((char *)(log->prefix), prefix);

#else
        strncpy((char *)(log->prefix), prefix,IOP_MAX_LOG_PATH);
#endif
	}
    log->fp = (FILE *)0;
    //io_lock_init(&(log->lock));
    __io_log_reopen(log);
    return 0;
}





int io_log_service_stop(io_log_service_t *logObj)
{
    if(!logObj){return 0;}
    //io_log_flush(logObj);
    //io_lock(&(logObj->lock));
    //20200829
    logObj->AscOrDec = -1;
    if((logObj->fp))
    {
        fclose(logObj->fp);
        logObj->fp = 0;
    }
    //20200827
    //io_thread_cancel((logObj->tid));
    //io_unlock(&(logObj->lock));
    //io_lock_destroy(&(logObj->lock));
    free(logObj);
    logObj = 0;
    return 0;
}


static int io_log_write(io_log_service_t *log,const void *buf, unsigned int len)
{
    int ret = 0;
    if(!log)
    {
        fwrite(buf,len, 1, stdout);
        return 0;
    }
    //io_lock(&(log->lock));
    //    if(log->log_buf_size + len <= IOP_MAX_LOG_BUF_SIZE)
    //    {
    //        memcpy(log->log_buf + log->log_buf_size, buf, len);
    //        log->log_buf_size += len;
    //        ret = (int)len;
    //    }
    //    else
    //    {
    //        if(!(log->fp))
    //        {
    //            __io_log_reopen(log);
    //        }
    //        if(log->fp)
    //        {
    //            if(log->log_buf_size > 0)
    //            {
    //                fwrite(log->log_buf, log->log_buf_size, 1, log->fp);
    //                log->log_buf_size = 0;
    //            }
    //            fwrite(buf, len,1, log->fp);
    //            //20200827
    //            fflush(log->fp);
    //            ret = (int)len;
    //        }
    //    }
    if(!(log->fp))
    {
        __io_log_reopen(log);
    }
    if(log->fp)
    {
        if(buf)
        {
            fwrite(buf, len, 1, log->fp);
        }
        fflush(log->fp);
        ret = (int)len;
    }
    //io_unlock(&(log->lock));
    return ret;
}





int LOG_DBG(io_log_service_t *logObj, const char *fmt,...)
{
    char buf[1024];
	int len = sizeof(buf)-1;
	int xlen = 0;
    io_log_service_t *log = logObj;
    struct tm t;
    time_t n ;
	int level = IOP_LOG_DBG;
	int r;
    if(!log || (level > log->log_level))
    {
        return -1;
    }
	buf[len] = 0;

	n= time(NULL);
    t = *localtime(&n);
    /*测试用，yanghui，0831*/
       r = snprintf(buf,len, "[DBG %02d:%02d:%02d] ",
                    t.tm_hour,t.tm_min, t.tm_sec);
    /*测试用，yanghui，0831*/
	{
	va_list ap;
    va_start(ap, fmt);

#ifdef WIN32
    xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
#else
	xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
#endif
	va_end(ap);
	}
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }
    return io_log_write(log,buf, r);

}

int LOG_DBG1(io_log_service_t *logObj, const char *buf1, int len1)
{
    char buf[1024];
    int len = sizeof(buf)-1;
    io_log_service_t *log = logObj;
    int level = IOP_LOG_DBG;
    if(!log || (level > log->log_level))
    {
        return -1;
    }
    buf[len] = 0;

    memcpy((char *)buf,buf1,len1);
    if(1 == log->AscOrDec)
    {
        len1 = AscToDec(buf,len1,1024);
    }
    buf[len1+1]=10; //"\n"
    buf[len1]=93;//"]"

    if(len1 + 2 > len)
    {
        len1 = len;
    }
    else
    {
        len1+=2;
    }
    return io_log_write(log,buf, len1);
}



int LOG_INF(io_log_service_t *logObj,const char *fmt,...)
{
    char buf[1024];
	int len = sizeof(buf)-1;
	int xlen = 0;
    io_log_service_t *log = logObj;
    struct tm t;
	int level = IOP_LOG_INFO;
    time_t n ;
	int r;
    if(!log || (level > log->log_level))
    {
        return -1;
    }
	buf[len] = 0;
	n= time(NULL);

    t = *localtime(&n);
    r = snprintf(buf,len, "[INF %02d:%02d:%02d] ",
                 t.tm_hour,t.tm_min, t.tm_sec);
	{
	va_list ap;
    va_start(ap, fmt);
	xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
	va_end(ap);
	}
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }
    return io_log_write(log,buf, r);

}


int LOG_ERR(io_log_service_t *logObj, const char *fmt,...)
{
    char buf[1024];
	int len = sizeof(buf)-1;
	int xlen = 0;
    io_log_service_t *log = logObj;
    struct tm t;
	int level = IOP_LOG_ERR;
    time_t n ;
	int r;
    if(!log || (level > log->log_level))
    {
        return -1;
    }
	buf[len] = 0;
	n= time(NULL);

    t = *localtime(&n);
    r = snprintf(buf,len, "[ERR %02d:%02d:%02d] ",
                 t.tm_hour,t.tm_min, t.tm_sec);
    {
	va_list ap;
    va_start(ap, fmt);

	xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
	va_end(ap);
	}
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }
    return io_log_write(log,buf, r);

}


int LOG_ALT(io_log_service_t *logObj,const char *fmt,...)
{
    char buf[1024];
	int len = sizeof(buf)-1;
	int xlen = 0;
    io_log_service_t *log = logObj;
    struct tm t;
	int level = IOP_LOG_ALERT;
    time_t n ;
	int r;
    if(!log || (level > log->log_level))
    {
        return -1;
    }
	buf[len] = 0;
	n= time(NULL);

    t = *localtime(&n);
    r = snprintf(buf,len, "[ALT %02d:%02d:%02d] ",
                 t.tm_hour,t.tm_min, t.tm_sec);
	{
	va_list ap;
    va_start(ap, fmt);

	xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
	va_end(ap);
	}
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }
    return io_log_write(log,buf, r);

}






int io_log_fmt(io_log_service_t *logObj,void *buf, int len, int level, const char *fmt,...)
{
	char *p = 0;
    int xlen = 0;
    const char *level_str_alert = "ALT";
    const char *level_str_err = "ERR";
    const char *level_str_dbg = "DBG";
    const char *level_str_info = "INF";
    const char *level_str = "LOG";
    io_log_service_t *log = logObj;
    struct tm t;
	time_t n;		
	int r;
	if(log && (level > log->log_level))
    {
        return -1;
    }
    p = (char *)buf;
    p[len-1] = 0;
    len -= 1;
    switch(level)
    {
    case IOP_LOG_ALERT:
            level_str = level_str_alert;
            break;
    case IOP_LOG_ERR:
            level_str = level_str_err;
            break;
    case IOP_LOG_DBG:
            level_str = level_str_dbg;
            break;
    case IOP_LOG_INFO:
            level_str = level_str_info;
            break;
    }
    n = time(NULL);


    t = *localtime(&n);
    r = snprintf((char *)buf,len, "[%s %02d:%02d:%02d] ", level_str, t.tm_hour,t.tm_min, t.tm_sec);
	{
	va_list ap;
    va_start(ap, fmt);


	xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
	
	va_end(ap);
	}
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }
    return io_log_write(log,buf, r);
}

/*
int io_log(int level, const char *fmt,...)
{
    int r = 0;
    char buf[10240];
    va_list ap;
    va_start(ap, fmt);
    r = io_log_fmt(buf,sizeof(buf),level,fmt,ap);
    va_end(ap);
    return r;    
}
*/



int LOG_DBG2(io_log_service_t *logObj, int q_hour, int q_min, int q_sec, int q_msec, const char *buf1, int len1, const char *fmt,va_list ap)
{
    char buf[1024];
    int len = sizeof(buf)-1;
    int xlen = 0;
    io_log_service_t *log = logObj;
    int level = IOP_LOG_DBG;
    int r;
    if(!log || (level > log->log_level))
    {
        return -1;
    }
    buf[len] = 0;
    r = snprintf(buf,len, "[DBG %02d:%02d:%02d:%03d] ",
                    q_hour,q_min,q_sec,q_msec);

    {
    //va_list ap;
    //va_start(ap, fmt);

#ifdef WIN32
    xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
#else
    xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
#endif
    //va_end(ap);
    }
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }

    if(len1 > 0)
    {
        memcpy((char *)buf+r,buf1,len1);
    }
    if(1 == log->AscOrDec && len1 > 0)
    {
        len1 = AscToDec(buf+r,len1,1024-r);
        if(-1 == len1)
            return -1;
    }
    buf[r+len1+1]=10; //"\n"
    buf[r+len1]=93;//"]"

    if(r+len1 + 2 > len)
    {
        len1 = len;
    }
    else
    {
        len1 = len1 + r + 2;
    }
    return io_log_write(log,buf, len1);
}
