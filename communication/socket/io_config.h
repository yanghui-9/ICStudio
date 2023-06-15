#ifndef _IOP_CONFIG_H_
#define _IOP_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef _NO_SELECT_
    #define _HAVE_SELECT_
#endif

#ifdef WIN32
   #include <winsock2.h>
   #include <ws2tcpip.h>
    #define WIN32_LEAN_AND_MEAN
    typedef SOCKET io_handle_t;
    #define INVALID_HANDLE INVALID_SOCKET
    #define socket_error     SOCKET_ERROR
    #define socket_errno	WSAGetLastError()
    #define SOCKET_EINTR	WSAEINTR
    #define SOCKET_EAGAIN	WSAEINPROGRESS
    #define SOCKET_EWOULDBLOCK WSAEWOULDBLOCK
#else

#ifndef _NO_EPOLL_
    #define _HAVE_EPOLL_
#endif

    #include <sys/types.h>
    #include <sys/socket.h>
   #include <arpa/inet.h>
    #include <unistd.h>
    typedef int io_handle_t;
    #define INVALID_HANDLE -1
    #define socket_error -1
    #define socket_errno errno
    #define SOCKET_EINTR	EINTR   
    #define SOCKET_EAGAIN	EAGAIN
    #define SOCKET_EWOULDBLOCK EWOULDBLOCK
    #define SOCKET_EADDRINUSE EADDRINUSE

#endif

typedef unsigned char io_uint8_t;
typedef unsigned short io_uint16_t;
typedef unsigned int io_uint32_t;
typedef unsigned long long io_uint64_t;

typedef char io_int8_t;
typedef short io_int16_t;
typedef int io_int32_t;
typedef long long io_int64_t;

typedef struct {
     io_int32_t y;
     io_int32_t m;
     io_int32_t d;
     io_int32_t h;
     io_int32_t min;
     io_int32_t s;
     io_int32_t msc;
}io_date_time;

#define IOP_MAKE_UINT64(h,l) ((io_uint64_t)(h)<<32|(l))
#define IOP_UINT64_HIGH(t) (t>>32)
#define IOP_UINT64_LOW(t) (t&(io_uint64_t)0xFFFFFF)

    
#define IOP_OK                          0
#define IOP_ERR                  -1
#define IOP_ERR_NO_MEM          -2
#define IOP_ERR_FULL                -3
#define IOP_ERR_EMPTY             -4
#define IOP_ERR_NOT_FOUND    -5

#ifdef __cplusplus
}
#endif
#endif

