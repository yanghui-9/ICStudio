#ifndef PROTOCOL_INTERFACE_H
#define PROTOCOL_INTERFACE_H
#include "service.h"

//协议具体实现抽象类
class Protocol_Interface
{
public:
    virtual ~Protocol_Interface(){}

    ////下面4个必须实现
    //协议参数初始化
    virtual void Init() = 0;

    //组建读值帧
    virtual int32_t ReadFrame(const std::vector<Protocol::AddrInfo> &addrInfo,Protocol::DataWCallback callback = nullptr) = 0;

    //组建写入帧
    virtual int32_t WriteFrame(const std::vector<Protocol::AddrInfoForRW> &addrInfo) = 0;

    //校验帧格式，比如握手帧的处理也可以在里面进行
    //返回值：error:表示错误重新开始,retry：表示再次接收一次
    virtual int32_t CheckFrame(Frame & recvF) = 0;

    //版本号
    virtual int32_t GetVersion(std::string &sVersion) = 0;




    ////下面的6个接口有需要就重写，一般用默认就行
    //地址转换
    virtual int32_t AddrS2D(const std::string &reg,const std::string &sIndex,uint64_t &index);
    virtual int32_t AddrD2S(const std::string &reg,const uint64_t &index,std::string &sIndex);

    //处理字节序(有默认处理代码，如果有需要，再在具体协议实现类中重写)
    virtual void DealByteOrder(char * value, const uint32_t &dataType);

    //解析返回帧数据部分特殊处理（有特殊处理的部分，如果有需要，再在具体协议实现类中重写）
    //返回-1表示子类已经解析完成，不需要再次解析
    //size单位是位
    virtual int32_t DealRecvData(const std::string &reg, uint64_t index,char * data, uint32_t &begin,uint32_t &size);

    //从站解析数据接口
    virtual void DealDataOfSlave(char * data,uint32_t len, char **sendBuff,uint32_t *slen);

    //判断两个地址块是否可以合并
    virtual bool JudgeAddrBlockMerge(Protocol::AddrInfo &addr0, Protocol::AddrInfo &addr1, int32_t intervallen, int32_t maxLen);

    //每次通讯之前掉用接口
    virtual void BeforeCommunication(){}
};

#endif // PROTOCOL_INTERFACE_H
