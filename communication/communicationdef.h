#ifndef COMMUNICATIONDEF
#define COMMUNICATIONDEF
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>
#include <functional>
#include "communicationglobal.h"


class ICommunication
{
public:
    using ResponseCallBack = std::function<void(int, char *,int)>;

    using DisconnectCB = std::function<void(const std::string &,int)>;
    using ConnectCB = std::function<void(const std::string &,int)>;
    using ReadReadyCB = std::function<int(int, char *,uint32_t,char **,uint32_t*)>;

    enum Communication_Type
    {
        serial = 0,
        tcp_client,
        tcp_server,
        upd
    };

    ICommunication(){}
    virtual ~ICommunication(){}
    /**
     * @brief   发送数据
     * @note
     * @return 发送成功返回发送成功的字节长度，发送失败返回-1
     */
    virtual int32_t SendData(char * pBuffer,  uint32_t iLength) = 0;

    /**
     * @brief    接收数据，按照指定长度和指定的接收方式
     * @param iLength接收最大长度，uiDelayTime超时时间
     * @return  接收成功时返回接收的长度，接收失败返回-1
     */
    virtual int32_t RecvData(char * pBuffer,  int32_t &iLength, uint32_t uiDelayTime) = 0;

    /**
     * @brief    写之后异步读一次
     * @param
     * @return
     */
    virtual int32_t AsynWR(char *sendBuf,int len,const ResponseCallBack &cb,
                           int readLenMax,int timeout) = 0;


    /**
     * @brief   异步接口，配合回调使用
     * @param   iLock 获取异步操作互斥锁，-1默认不做锁操作正常调用，0-释放锁，1-获取锁
     * @return  成功0，失败-1
     */
    virtual int32_t AsynProcess(int32_t iLock = -1) = 0;


    /**
     * @brief     打开通讯日志记录
     * @note     AscOrDec =0 表示asc协议，AscOrDec = 1 表示dec协议
     * @param  openType,0-w,1-w+,2-a,3-a+
     * @return  成功0，失败-1
     */
    virtual int32_t OpenLog(const char * LogName,int AscOrDec =0,int OpenType = 2) = 0;

    /**
     * @brief    关闭通讯日志记录
     * @return  成功0，失败-1
     */
    virtual int32_t CloseLog() = 0;

    /**
     * @brief     开启通讯
     * @note     是否设置回调函数-接受连接-接收数据处理可变参数(服务器端)
     * @note     tcp客户端连接时阻塞连接的方式setnonblock =1，为非阻塞式连接返回值无效
     * @note     tcp客户端连接时阻塞连接的方式setnonblock =0, 阻塞式
     * @return  成功0，>0错误码
     */
    virtual int32_t OpenComm(int setnonblock = 0,int Timo = 500,
                             const ConnectCB &connectCB = nullptr,
                             const DisconnectCB &disconnectCB = nullptr,
                             const ReadReadyCB &ReadReadyCB = nullptr) = 0;

    /**
     * @brief    关闭通讯
     * @return  成功0，失败-1
     */
    virtual int32_t CloseComm() = 0;

    /**
     * @brief  设置串口通信参数
     * @note   1-9600-7-E-1-10-0，串口分别对应串口号-波特率-数据位-校验位-停止位-串口轮询时间-流控（0-无流控，1-硬件流控，2-软件流控）
     * @note   192.168.1.100-8888，客户端分别对应ip地址-端口号
     * @note   8888，服务端只有端口号
     * @return 成功返回0，失败-1
     */
    virtual int32_t SetCommInfo(const std::string &sPar) = 0;

    /**
     * @brief    获取串口通信参数
     * @return  成功返回0，失败-1
     */
    virtual int32_t GetCommInfo(std::string &sPar) = 0;
};


/**
 * @brief   创建通讯对象
 * @return  成功返回对象指针，失败null
 *          1-9600-7-E-1-10-0，串口分别对应串口号-波特率-数据位-校验位-停止位-串口轮询时间-流控（0-无流控，1-硬件流控，2-软件流控）
 *          192.168.1.100-8888，客户端分别对应ip地址-端口号
 *          8888，服务端只有端口号
 */
_API_COMM ICommunication * CreateComm(ICommunication::Communication_Type type, const std::string &sPar);


/**
 * @brief    关闭通讯对象，并且释放对象资源
 * @return  成功返回0，失败-1
 */
_API_COMM int CloseCommunication(ICommunication *pCommunication);

#endif // COMMUNICATIONDEF

