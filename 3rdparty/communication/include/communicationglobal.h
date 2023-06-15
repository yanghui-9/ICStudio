#ifndef COMMUNICATION_GLOBAL_H
#define COMMUNICATION_GLOBAL_H
#include <stdint.h>
#include <string>


#define CB_CLASS_BIND(Name, Class , object, arg) \
template<class desttype> \
int32_t Name##_Bind(desttype* pclass, void (desttype::*pmemfun)arg ,bool isBind = false) \
{ \
    if(!isBind) \
    { \
        if(object) \
        { \
            delete  object; \
            object = nullptr; \
        } \
        object = new Class<desttype>(pclass, pmemfun); \
        return 0; \
    } \
    else \
    { \
        if(object) \
        { \
            delete  object; \
            object = nullptr; \
        } \
        return 0; \
    } \
}

#define CB_BIND(Name, Class, num, object) \
int32_t Name##_Bind(socktCB##num cb ,bool isBind = false) \
{ \
    if(!isBind) \
    { \
        if(object) \
        { \
            delete  object; \
            object = nullptr; \
        } \
        object = new Class(cb); \
        return 0; \
    } \
    else \
    { \
        if(object) \
        { \
            delete  object; \
            object = nullptr; \
        } \
        return 0; \
    } \
}

//客户端链接回调
using socktCB0 = void (*)(const std::string &,int32_t,int32_t&);
//客户端数据准备&断开链接
using socktCB1 = void (*)(int32_t,int32_t);
//客户端断开链接
using socktCB2 = void (*)(int32_t);


class __connection_base0
{
public:
    virtual ~__connection_base0(){}
    virtual void connect(const std::string &clientIP, int32_t iPort, int32_t &clientID) = 0;
    virtual bool isEqual(void *p0) = 0;
};
class __connection_base1
{
public:
    virtual ~__connection_base1(){}
    virtual void readReady(int32_t clientID,int len) = 0;
    virtual bool isEqual(void *p0) = 0;
};
class __connection_base2
{
public:
    virtual ~__connection_base2(){}
    virtual void disconnect(int32_t clientID) = 0;
    virtual bool isEqual(void *p0) = 0;
};
template<class dest_type>
class __connection00: public __connection_base0
{
public:
    __connection00(dest_type* pobject, void (dest_type::*pmemfun)(const std::string,int,int32_t &))
    {
        m_pobject = pobject;
        m_pmemfun = pmemfun;
    }
    virtual void connect(std::string &clientIP, int32_t iPort, int32_t &clientID)
    {
        if(m_pobject)
        {
            (m_pobject->*m_pmemfun)(clientIP,iPort,clientID);
        }
    }
    virtual bool isEqual(void *p0)
    {
        if(p0 == m_pobject)
        {
            return true;
        }
        return  false;
    }

private:
    dest_type* m_pobject = nullptr;
    void (dest_type::* m_pmemfun)(std::string,int,int32_t &) = nullptr;
};

class __connection01: public __connection_base0
{
public:
    __connection01(socktCB0 CB0){m_CB0 = CB0;}
    virtual void connect(const std::string &clientIP, int32_t iPort, int32_t &clientID)
    {
        if(m_CB0){m_CB0(clientIP,iPort,clientID);}
    }
    virtual bool isEqual(void *p0)
    {
        if(p0 == reinterpret_cast<void *>( m_CB0 ))
        {
            return true;
        }
        return  false;
    }
private:
    socktCB0 m_CB0 = nullptr;
};

template<class dest_type>
class __connection10: public __connection_base1
{
public:
    __connection10(dest_type* pobject1, void (dest_type::*pmemfun1)(int32_t,int32_t)){m_pobject1 = pobject1;m_pmemfun1 = pmemfun1;}
    virtual void readReady(int32_t clientID,int32_t len)
    {
        if(m_pobject1)
        {
            (m_pobject1->*m_pmemfun1)(clientID,len);
        }
    }
    virtual bool isEqual(void *p0)
    {
        if(p0 == m_pobject1)
        {
            return true;
        }
        return  false;
    }
private:
    dest_type* m_pobject1 = nullptr;
    void (dest_type::* m_pmemfun1)(int,int) = nullptr;
};

class __connection11: public __connection_base1
{
public:
    __connection11(socktCB1 CB1){m_CB1 = CB1;}
    virtual void readReady(int32_t clientID,int len)
    {
        if(m_CB1){m_CB1(clientID,len);}
    }
    virtual bool isEqual(void *p0)
    {
        if(p0 == reinterpret_cast<void *>(m_CB1) )
        {
            return true;
        }
        return  false;
    }
private:
    socktCB1 m_CB1 = nullptr;
};

template<class dest_type>
class __connection20: public __connection_base2
{
public:
    __connection20(dest_type* pobject2, void (dest_type::*pmemfun2)(int32_t))
    {
        m_pobject2 = pobject2;
        m_pmemfun2 = pmemfun2;
    }
    virtual void disconnect(int32_t clientID)
    {
        if(m_pobject2)
        {
            (m_pobject2->*m_pmemfun2)(clientID);
        }
    }
    virtual bool isEqual(void *p0)
    {
        if(p0 == m_pobject2)
        {
            return true;
        }
        return  false;
    }
private:
    dest_type* m_pobject2 = nullptr;
    void (dest_type::* m_pmemfun2)(int) = nullptr;
};

class __connection21: public __connection_base2
{
public:
    __connection21(socktCB2 CB2)
    {
        m_CB2 = CB2;
    }
    virtual void disconnect(int32_t clientID)
    {
        if(m_CB2){m_CB2(clientID);}
    }
    virtual bool isEqual(void *p0)
    {
        if(p0 == reinterpret_cast<void *>(m_CB2) )
        {
            return true;
        }
        return  false;
    }
private:
    socktCB2 m_CB2 = nullptr;
};


#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define Q_OS_DARWIN
#  define Q_OS_BSD4
#  ifdef __LP64__
#    define Q_OS_DARWIN64
#  else
#    define Q_OS_DARWIN32
#  endif
#elif defined(__SYMBIAN32__) || defined(SYMBIAN)
#  define Q_OS_SYMBIAN
#  define Q_NO_POSIX_SIGNALS
#  define QT_NO_GETIFADDRS
#elif defined(__CYGWIN__)
#  define Q_OS_CYGWIN
#elif defined(MSDOS) || defined(_MSDOS)
#  define Q_OS_MSDOS
#elif defined(__OS2__)
#  if defined(__EMX__)
#    define Q_OS_OS2EMX
#  else
#    define Q_OS_OS2
#  endif
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define Q_OS_WIN32
#  define Q_OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  if defined(WINCE) || defined(_WIN32_WCE)
#    define Q_OS_WINCE
#  else
#    define Q_OS_WIN32
#  endif
#elif defined(__MWERKS__) && defined(__INTEL__)
#  define Q_OS_WIN32
#elif defined(__sun) || defined(sun)
#  define Q_OS_SOLARIS
#elif defined(hpux) || defined(__hpux)
#  define Q_OS_HPUX
#elif defined(__ultrix) || defined(ultrix)
#  define Q_OS_ULTRIX
#elif defined(sinix)
#  define Q_OS_RELIANT
#elif defined(__native_client__)
#  define Q_OS_NACL
#elif defined(__linux__) || defined(__linux)
#  define Q_OS_LINUX
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#  define Q_OS_FREEBSD
#  define Q_OS_BSD4
#elif defined(__NetBSD__)
#  define Q_OS_NETBSD
#  define Q_OS_BSD4
#elif defined(__OpenBSD__)
#  define Q_OS_OPENBSD
#  define Q_OS_BSD4
#elif defined(__bsdi__)
#  define Q_OS_BSDI
#  define Q_OS_BSD4
#elif defined(__sgi)
#  define Q_OS_IRIX
#elif defined(__osf__)
#  define Q_OS_OSF
#elif defined(_AIX)
#  define Q_OS_AIX
#elif defined(__Lynx__)
#  define Q_OS_LYNX
#elif defined(__GNU__)
#  define Q_OS_HURD
#elif defined(__DGUX__)
#  define Q_OS_DGUX
#elif defined(__QNXNTO__)
#  define Q_OS_QNX
#elif defined(_SEQUENT_)
#  define Q_OS_DYNIX
#elif defined(_SCO_DS) /* SCO OpenServer 5 + GCC */
#  define Q_OS_SCO
#elif defined(__USLC__) /* all SCO platforms + UDK or OUDK */
#  define Q_OS_UNIXWARE
#elif defined(__svr4__) && defined(i386) /* Open UNIX 8 + GCC */
#  define Q_OS_UNIXWARE
#elif defined(__INTEGRITY)
#  define Q_OS_INTEGRITY
#elif defined(VXWORKS) /* there is no "real" VxWorks define - this has to be set in the mkspec! */
#  define Q_OS_VXWORKS
#elif defined(__MAKEDEPEND__)
#else
#  error "Qt has not been ported to this OS - talk to qt-bugs@trolltech.com"
#endif

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_WINCE)
#  define Q_OS_WIN
#endif

#if defined(Q_OS_DARWIN)
#  define Q_OS_MAC /* Q_OS_MAC is mostly for compatibility, but also more clear */
#  define Q_OS_MACX /* Q_OS_MACX is only for compatibility.*/
#  if defined(Q_OS_DARWIN64)
#     define Q_OS_MAC64
#  elif defined(Q_OS_DARWIN32)
#     define Q_OS_MAC32
#  endif
#endif

#ifdef QT_AUTODETECT_COCOA
#  ifdef Q_OS_MAC64
#    define QT_MAC_USE_COCOA 1
#    define QT_BUILD_KEY QT_BUILD_KEY_COCOA
#  else
#    define QT_BUILD_KEY QT_BUILD_KEY_CARBON
#  endif
#endif

#if defined(Q_WS_MAC64) && !defined(QT_MAC_USE_COCOA) && !defined(QT_BUILD_QMAKE) && !defined(QT_BOOTSTRAPPED)
#error "You are building a 64-bit application, but using a 32-bit version of Qt. Check your build configuration."
#endif

#if defined(Q_OS_MSDOS) || defined(Q_OS_OS2) || defined(Q_OS_WIN)
#  undef Q_OS_UNIX
#elif !defined(Q_OS_UNIX)
#  define Q_OS_UNIX
#endif

#if defined(Q_OS_WIN)	/* { */
#if defined(COMMUNICATION_LIBRARY)	/* { */
#define _API  extern "C" __declspec(dllexport)
#else
#define _API  extern "C" __declspec(dllimport)
#endif
#else
#define _API	 extern
#endif


#endif // COMMUNICATION_GLOBAL_H
