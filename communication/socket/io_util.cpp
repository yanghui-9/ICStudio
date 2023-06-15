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
#include "io_util.h"
#include "io_log_service.h"


io_handle_t io_socket_datagram();                                                


int io_close_handle(io_handle_t h)
{
    int r = 0;
#ifndef WIN32
    do{r = close(h);}while( (r == -1) && (errno == EINTR));
#else
    r = closesocket(h);
#endif
    return r;
}

int io_read(io_handle_t h, void *buf, unsigned int count)
{
    int n = 0;
    do{
    #ifdef WIN32
        n = recv(h, (char *)buf, count,0);
    #else
        n = read(h,(char *)buf, count);
    #endif
    }while((n < 0) && (socket_errno == EINTR));
    return n;    
}

int io_write(io_handle_t h, const void *buf, unsigned int count)
{
    int n = 0;
    do{
    #ifdef WIN32
        n = send(h, (char *)buf, count,0);
    #else
        n = write(h,(char *)buf, count);
    #endif
    }while((n < 0) && (socket_errno == EINTR));
//    if(n>0)
//    {
//        LOG_DBG("write = %d[", n);
//        LOG_DBG1((char *)buf,n);
//    }
    return n;    
}



int io_readv(io_handle_t h, iov_type *vec, int count)
{
    int n = 0;
#ifdef WIN32
       DWORD bytesRead = 0;
       DWORD flags = 0;
    if (WSARecv(h, vec, count, &bytesRead, &flags, NULL, NULL)) {
        /* The read failed. It might be a close,
         * or it might be an error. */
        if (WSAGetLastError() == WSAECONNABORTED)
            n = 0;
        else
            n = -1;
    } else
        n = bytesRead;

#else
    do{
        n = readv(h,vec, count);
    }while((n < 0) && (socket_errno == EINTR));

#endif
    return n;
}

int io_writev(io_handle_t h, const iov_type *iov, int count)    
{
    int n = 0;
#ifdef WIN32
    DWORD bytesSent=0;
    if (WSASend(h, (LPWSABUF)iov, count, &bytesSent, 0, NULL, NULL))
        n = -1;
    else
        n = bytesSent;
#else
    do{
        n = writev(h,iov, count);
    }while((n < 0) && (socket_errno == EINTR));

#endif
    return n;
}

int open_dev_null(int fd) {
    int tmpfd=-1;
#ifdef WIN32

#else
    close(fd);
    tmpfd = open("/dev/null", O_RDWR);
    if (tmpfd != -1 && tmpfd != fd) {
        dup2(tmpfd, fd);
        close(tmpfd);
    }
#endif

    return (tmpfd != -1) ? 0 : -1;
}




int io_set_nonblock(io_handle_t h)
{
#ifdef WIN32
    u_long mode = 1;
    return ioctlsocket(h,FIONBIO,&mode);
#else
    int val = fcntl(h, F_GETFL, 0);
    if (val == -1){return -1;}
    if (val & O_NONBLOCK){ return 0; }
    return fcntl(h, F_SETFL, val | O_NONBLOCK | O_NDELAY);
#endif    
}

int io_set_block(io_handle_t h)
{
#ifdef WIN32
    u_long mode = 0;
    return ioctlsocket(h,FIONBIO,&mode);
#else
    int val = fcntl(h, F_GETFL, 0);
    if (val == -1){return -1;}
    if (val & O_NONBLOCK){ 
        return fcntl(h, F_SETFL, (val & ~O_NONBLOCK) | O_NDELAY);
    }
    return 0;
#endif    
}


int io_sock_reuseaddr(io_handle_t h)
{
    int optval = 1;
    unsigned optlen = sizeof(optval);
    int ret = setsockopt(h, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, optlen);
    return ret;
}


int io_set_sock_recv_timeo(io_handle_t sock, int ms)
{
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
}

int io_set_sock_snd_timeo(io_handle_t sock, int ms)
{
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv)); 
}



int io_sock_bind(io_handle_t sock, const char *ip, const unsigned short port)
{
    struct sockaddr_in inaddr;    
    memset (&inaddr, 0,sizeof (struct sockaddr_in));
    inaddr.sin_family = AF_INET;        //ipv4协议族    
    inaddr.sin_port = htons (port);    
       if((!ip) || (ip[0] == 0) || (strcmp(ip, "0.0.0.0") == 0))
    {
        inaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
           inaddr.sin_addr.s_addr = inet_addr(ip);
    }
    return bind(sock, (struct sockaddr*)&inaddr, sizeof(struct sockaddr));
}

io_handle_t io_socket_stream()
{
    return socket(PF_INET, SOCK_STREAM, 0);
}

io_handle_t io_socket_datagram()
{
    return socket(PF_INET, SOCK_DGRAM, 0);
}

io_handle_t io_tcp_server(const char *host, unsigned short port)
{
	int r = -1;
	int ret = -1;
    io_handle_t sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_HANDLE)
    {
        return INVALID_HANDLE;
    }

    io_sock_reuseaddr(sock);
    ret = io_sock_bind(sock, host, port);
    if(ret != 0)
    {
        io_close_handle(sock);
        return INVALID_HANDLE;
    }
    r = listen(sock, 32);
    if(r != 0)
    {
        io_close_handle(sock);
        return INVALID_HANDLE;
    }
    return sock;
}


io_handle_t io_udp_server(const char *host, unsigned short port)
{
    int ret = -1;
    io_handle_t sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == INVALID_HANDLE)
    {
        return sock;
    }
    io_sock_reuseaddr(sock);
    ret = io_sock_bind(sock, host, port);
    if(ret != 0)
    {
        io_close_handle(sock);
        return INVALID_HANDLE;
    }
    return sock;
}


int io_connect_timeo(io_handle_t sock, const io_sockaddr_in *addr, int timeout_usec)
{
	int n = 0;
	int error;
    struct timeval timeout;
    fd_set rset, wset;
	int ret = -1;
    io_socklen_t addr_len = sizeof(io_sockaddr_in);
    int retval = 0;
    if (timeout_usec < 0) // 阻塞
    {
        return connect(sock, (struct sockaddr *)addr, addr_len);
    }
       if(io_set_nonblock(sock) != 0)
       {
            return -1;
       }

        ret = retval= connect(sock, (struct sockaddr *) addr, addr_len);
        /*Bug 2448，1200协议，通讯成功后断开连接，重新连接通讯报错，yanghui，20180330，begin*/

        /*Bug 2448，1200协议，通讯成功后断开连接，重新连接通讯报错，yanghui，20180330，end*/

        if(retval == 0)
        {
            return 0;
        }

        if(timeout_usec == 0)   //timeout
        {
            return -1;
        }
        FD_ZERO(&rset);
        FD_SET(sock, &rset);
        FD_ZERO(&wset);
        FD_SET(sock, &wset);
        MAKE_TIMEVAL(timeout, timeout_usec);
        /*Bug 2448，1200协议，通讯成功后断开连接，重新连接通讯报错，yanghui，20180330，begin*/
        n = select(sock + 1, NULL, &wset, NULL, &timeout);
        /*Bug 2448，1200协议，通讯成功后断开连接，重新连接通讯报错，yanghui，20180330，begin*/
        if( n == 0) //timeout
        {
            return -1;
        }

        error = 0;
        if(FD_ISSET(sock, &rset) || FD_ISSET(sock, &wset))
        {
            io_socklen_t len = sizeof(error);
            if(getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&error, &len) < 0)
            {
                return -1;
            }

            if(error)
            {
                return -1;
            }
            return 0;
    }
    //not ready...
    return -1;
}


int io_connect(io_handle_t sock, const char *ip, unsigned short port)
{
    io_sockaddr_in    addr;
    io_socklen_t addrlen = sizeof(addr);
    SOCKADDR_IN(addr, ip, port);
    return  connect(sock,  (const  io_sockaddr *)&addr, addrlen);
}


int io_do_connect(io_handle_t sock, const io_sockaddr_in *addr)
{
      io_socklen_t addrlen = sizeof(addr);
      return  connect(sock,  (const io_sockaddr *)addr, addrlen);
}

io_handle_t io_accept(io_handle_t sock, io_sockaddr *addr, io_socklen_t *len)
{
    return accept(sock, addr, len);
}


int io_sendto(io_handle_t s, const void *buf, unsigned int len, int flags, const io_sockaddr *to, io_socklen_t tolen)
{
    return sendto(s,(char *)buf, len,flags, to,tolen);
}

int io_recvfrom(io_handle_t s, void *buf, unsigned int len, int flags, io_sockaddr *from, io_socklen_t *fromlen)
{
    return recvfrom(s,(char *)buf,len,flags,from,fromlen);
}


io_uint16_t io_htons(io_uint16_t x)
{
    return htons(x);
}

io_uint32_t io_htonl(io_uint32_t x)
{
    return htonl(x);
}

io_uint16_t io_ntohs(io_uint16_t x)
{
    return ntohs(x);
}

io_uint32_t io_ntohl(io_uint32_t x)
{
    return ntohl(x);
}

int io_send_n(io_handle_t s, const char *buf, int len)
{
    int result, old_len = len;
    do {
        result = io_write(s, buf, len);
        if (result == socket_error)
            return socket_error;
        len -= result;
        buf += result;
    } while (len);
    return old_len;
}


int io_recv_n(io_handle_t s, char *buf, int len)
{
    int result, old_len = len;
    do {
        result = io_read(s, buf, len);
	 if(result == 0){return 0;}
        if (result == socket_error)
            return socket_error;
        len -= result;
        buf += result;
    } while (len);
    return old_len;
}



void io_usleep(int usec)
{
#ifdef WIN32
    Sleep(usec);
#else
    usleep(usec);
#endif
}



char * io_getsockip(io_handle_t s)
{
    io_sockaddr_in addr;
    io_socklen_t  addrlen = sizeof(addr);
    if(getsockname(s,(io_sockaddr*)&addr,&addrlen) == 0)
    {
        return inet_ntoa(addr.sin_addr);
    }
    else
    {
        return 0;
    }
}


int io_getsockport(io_handle_t s)
{
    io_sockaddr_in addr;
    io_socklen_t  addrlen = sizeof(addr);
    if(getsockname(s,(io_sockaddr*)&addr,&addrlen) == 0)
    {
        return (int)ntohs(addr.sin_port);
    }
    else
    {
        return -1;
    }
}


io_uint32_t io_julianDayFromDate(int year, int month, int day)
{
    if (year < 0)
        ++year;

    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
    {
        return (1461 * (year + 4800 + (month - 14) / 12)) / 4
               + (367 * (month - 2 - 12 * ((month - 14) / 12))) / 12
               - (3 * ((year + 4900 + (month - 14) / 12) / 100)) / 4
               + day - 32075;
    }
    else if (year < 1582 || (year == 1582 && (month < 10 || (month == 10 && day <= 4))))
    {
        // Julian calendar until October 4, 1582
        // Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
        int a = (14 - month) / 12;
        return (153 * (month + (12 * a) - 3) + 2) / 5
               + (1461 * (year + 4800 - a)) / 4
               + day - 32083;
    }
    else
    {
        // the day following October 4, 1582 is October 15, 1582
        return 0;
    }
}

void getDateFromJulianDay(io_uint32_t julianDay, int *year, int *month, int *day)
{
    int y, m, d;

    if (julianDay >= 2299161) {
        // Gregorian calendar starting from October 15, 1582
        // This algorithm is from Henry F. Fliegel and Thomas C. Van Flandern
        io_uint64_t ell, n, i, j;
        ell = (io_uint64_t) julianDay + 68569;
        n = (4 * ell) / 146097;
        ell = ell - (146097 * n + 3) / 4;
        i = (4000 * (ell + 1)) / 1461001;
        ell = ell - (1461 * i) / 4 + 31;
        j = (80 * ell) / 2447;
        d = ell - (2447 * j) / 80;
        ell = j / 11;
        m = j + 2 - (12 * ell);
        y = 100 * (n - 49) + i + ell;
    } else {
        // Julian calendar until October 4, 1582
        // Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
        julianDay += 32082;
        int dd = (4 * julianDay + 3) / 1461;
        int ee = julianDay - (1461 * dd) / 4;
        int mm = ((5 * ee) + 2) / 153;
        d = ee - (153 * mm + 2) / 5 + 1;
        m = mm + 3 - 12 * (mm / 10);
        y = dd - 4800 + (mm / 10);
        if (y <= 0)
            --y;
    }
    if (year)
        *year = y;
    if (month)
        *month = m;
    if (day)
        *day = d;
}

io_uint64_t io_getCurTimeToMsec()
{
#ifdef WIN32
      SYSTEMTIME st;
      memset(&st, 0, sizeof(SYSTEMTIME));
      GetLocalTime(&st);
      io_uint64_t tmpDate = io_julianDayFromDate(st.wYear, st.wMonth, st.wDay);
      io_uint64_t tmpTime = st.wHour*60*60*1000+ st.wMinute*60*1000+st.wSecond*1000+ st.wMilliseconds;
      return (tmpDate-2440588)*24*60*60*1000+tmpTime;
#else
    struct timeval time1;
    gettimeofday(&time1,NULL);
    return (io_uint64_t)time1.tv_sec *1000 + time1.tv_usec/1000;
#endif
}

io_date_time io_getcurDateTime()
{
    io_date_time tmpDateTime;
#ifdef WIN32
      SYSTEMTIME st;
      memset(&st, 0, sizeof(SYSTEMTIME));
      GetLocalTime(&st);
      tmpDateTime.y = st.wYear;
      tmpDateTime.m = st.wMonth;
      tmpDateTime.d = st.wDay;
      tmpDateTime.h = st.wHour;
      tmpDateTime.min = st.wMinute;
      tmpDateTime.s = st.wSecond;
      tmpDateTime.msc = st.wMilliseconds;
#else
    time_t timep;
    struct tm  p;
    time(&timep);
    p=*localtime(&timep);
    tmpDateTime.y = p.tm_year+1900;
    tmpDateTime.m = p.tm_mon;
    tmpDateTime.d = p.tm_mday;
    tmpDateTime.h = p.tm_hour;
    tmpDateTime.min = p.tm_min;
    tmpDateTime.s = p.tm_sec;
    struct timeval time1;
    gettimeofday(&time1,NULL);
    tmpDateTime.msc = time1.tv_usec/1000;
#endif
    return tmpDateTime;
}


io_date_time io_getDateTimeFromMsec(io_uint64_t msec)
{
    int year,month,day;
    io_date_time tmpDateTime;
    int ddays = msec / 86400000;
    msec %= 86400000;
    getDateFromJulianDay(ddays,&year,&month,&day);
    tmpDateTime.y = year;
    tmpDateTime.m = month;
    tmpDateTime.d = day;
    tmpDateTime.h =  msec/(60*60*1000);
    msec %= 3600000;
    tmpDateTime.min = msec/(60*1000);
    msec %= 60000;
    tmpDateTime.s = msec/1000;
    msec %= 1000;
    tmpDateTime.msc = msec;
    return tmpDateTime;
}


io_uint64_t io_getIntervalMsec(io_date_time &t1, io_date_time &t2)
{
    return io_getMsecFromDateTime(t1) - io_getMsecFromDateTime(t2);
}


io_uint64_t io_getMsecFromDateTime(io_date_time &t1)
{
      io_uint64_t tmpDate = io_julianDayFromDate(t1.y, t1.m, t1.d);
      io_uint64_t tmpTime = t1.h*60*60*1000+ t1.min*60*1000+ t1.s*1000+ t1.msc;
      return (tmpDate-2440588)*24*60*60*1000+tmpTime;
}


int io_getsockpro(io_handle_t s, char *ip, int *port)
{
    io_sockaddr_in addr;
    io_socklen_t  addrlen =sizeof(addr);
    if(getsockname(s,(io_sockaddr*)&addr,&addrlen) == 0)
    {
        char * ip1 =  inet_ntoa(addr.sin_addr);
        strcpy(ip,ip1);
        *port =  (int)ntohs(addr.sin_port);
    }
    else
    {
        return -1;
    }
}
