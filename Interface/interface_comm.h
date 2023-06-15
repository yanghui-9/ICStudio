#ifndef INTERFACE_COMM_H
#define INTERFACE_COMM_H
#include <string>
#include <functional>
#include <variant>
#include "protocol_type.h"

struct LinkValue
{
   std::string Name;//连接名称.
   std::string Manufacturer;//厂商名称.
   std::string Device_Name;//设备协议名称.
   Protocol::Device_Config config;//协议配置.
};

class interface_comm
{
public:
    using ResponseCallBack = std::function<void(int, char *,int)>;
    using DisconnectCB = std::function<void(const std::string &,int)>;
    using ConnectCB = std::function<void(const std::string &,int)>;
    using ReadReadyCB = std::function<int(int, char *,uint32_t,char **,uint32_t*)>;

    virtual ~interface_comm(){}

    //初始化所有连接
    virtual int Init() = 0;
    //卸载所有连接
    virtual int UnInit() = 0;

    //关闭启动指定连接
    virtual int CloseLink(const std::string &linkName) = 0;
    virtual int StartLink(const std::string &linkName) = 0;

    //动态获取通讯连接配置
    virtual int GetLinkConfig(const std::string &linkName,LinkValue &linkV) = 0;
    virtual int SetLinkConfig(const std::string &linkName,const LinkValue &linkV) = 0;


    ////绑定了通讯协议的通讯连接使用（变量地址或者标签模式）
    virtual int GetValue(Protocol::AddrInfoForRW &addr,const Protocol::Read_Opt_Type &RType) = 0;

    virtual int SetValue(Protocol::AddrInfoForRW &addr, const Protocol::Write_Opt_Type &WType) = 0;

    virtual int SetCycReadAddrinfo(const std::string &linkName,
                                   const std::vector<Protocol::AddrInfoForRW> &addrVec,
                                   Protocol::Add_Type addType) = 0;

    virtual int SetAddrValueChangeCB(const std::string &linkName,
                                     const std::vector<Protocol::AddrInfoForRW> &addrVec,
                                     Protocol::Add_Type addType) = 0;
    //标签操作
    virtual int32_t GetTagList(const std::string &linkName,std::vector<Protocol::Tag_Info> &tagList) = 0;
    virtual int32_t SetTagList(const std::string &linkName,const std::vector<Protocol::Tag_Info> &tagList) = 0;



    //通讯连接未绑定通讯协议可以直接使用通讯接口
    virtual int  WriteAndRead(const std::string &linkName,char *sendBuf,int len,
                              const ResponseCallBack &cb,int timeout, int recvLenMax) = 0;
    virtual int  Write(const std::string &linkName,char *buf,int len) = 0;
    virtual int  Read(const std::string &linkName,char *buf,int len) = 0;
    //Tcp服务端可注册事件回调来通讯
    virtual int  SetCallback(const std::string &linkName,const ConnectCB &connectCB = nullptr,
                             const DisconnectCB &disconnectCB = nullptr,
                             const ReadReadyCB &ReadReadyCB = nullptr) = 0;
};




#endif // INTERFACE_COMM_H
