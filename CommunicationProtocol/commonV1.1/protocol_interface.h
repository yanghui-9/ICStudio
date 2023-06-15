#ifndef PROTOCOL_INTERFACE_H
#define PROTOCOL_INTERFACE_H
#include "service.h"


//协议具体实现抽象类.
class Protocol_Interface
{
public:
    virtual ~Protocol_Interface(){}

    /*****下面5个必须实现*****/
    /**
    *  @brief  初始化一些协议默认参数.
    */
    virtual void Init() = 0;

    /**
    *  @brief  组建读值帧.
    *  @param  addrInfo ：地址或者标签信息.
    *  @return 返回结果，>0 --表示成功和组帧的个数  0 --失败.
    */
    virtual int32_t ReadFrame(const std::vector<AddrInfo> &addrInfo) = 0;


    /**
    *  @brief  组建数据写入帧.
    *  @param  addrInfo ：需要写入的地址或者标签信息和对应的写入数据信息.
    *  @return 返回结果，>0 --表示成功和组帧的个数  0 --失败.
    */
    virtual int32_t WriteFrame(const std::vector<Protocol::AddrInfoForRW> &addrInfo) = 0;


    /**
    *  @brief  校验帧格式，比如握手帧的处理也可以在里面进行.
    *  @param  recvF ：当前正常处理的帧（里面包含发送和接收到的数据等等）.
    *  @return error：表示错误重新开始, retry：表示再次接收一次.
    */
    virtual int32_t CheckFrame(Frame & recvF) = 0;

    /**
    *  @brief  版本号.
    *  @param  sVersion ：返回的版本号.
    *  @return 返回结果，-1 ： 失败 0  ：成功.
    */
    virtual int32_t GetVersion(std::string &sVersion) = 0;




    /*****下面的7个接口有需要就重写，一般用默认就行*****/
    /**
    *  @brief  获取全局帧，比如握手帧.
    */
    virtual void GetGlobalFrame();

    /**
    *  @brief  地址转换，字符串显示型地址和数值型地址相互转换.
    *  @param  reg：寄存器名称  sIndex：字符串地址 index：数值地址.
    *  @return 返回结果，-1 ： 失败 0  ：成功.
    */
    virtual int32_t AddrS2D(const std::string &reg,const std::string &sIndex,uint64_t &index);
    virtual int32_t AddrD2S(const std::string &reg,const uint64_t &index,std::string &sIndex);

    /**
    *  @brief  处理字节序(有默认处理代码，如果有需要，再在具体协议实现类中重写).
    *  @param  addrinfo：地址信息（包含数据长度和数据起始）  data：数据.
    *  @return 返回判断合并结果，-1 ：地址合并 0  ：地址块不合并.
    */
    virtual void DealByteOrder(char * value, const uint32_t &dataType);


    /**
    *  @brief  解析返回帧数据部分特殊处理（有特殊处理的部分，如果有需要，再在具体协议实现类中重写）.
    *  @param  addrinfo：地址信息（包含数据长度和数据起始）  data：数据.
    *  @return 返回判断合并结果，-1 ：地址合并 0  ：地址块不合并.
    */
    virtual int32_t DealRecvData(AddrInfo &addrinfo, char * data);


    /**
    *  @brief  从站解析数据和组帧回复数据.
    *  @param  data：接收到的数据  len：接收的数据长度  sendBuff：回复的数据 slen：回复的数据的数据长度.
    */
    virtual void DealDataOfSlave(char * data,uint32_t len, char **sendBuff,uint32_t *slen);


    /**
    *  @brief  判断两个地址块是否可以合并.
    *  @param  addr0：当前地址块  addr1：待合并地址块  intervallen：最大地址间隔 maxLen：最大块长度.
    *  @return 返回判断合并结果.
    *  --true 地址合并.
    *  --false 地址块不合并.
    */
    virtual bool JudgeAddrBlockMerge(AddrInfo &addr0, AddrInfo &addr1, int32_t intervallen, int32_t maxLen);


    /**
    *  @brief  每次通讯之前掉用接口.
    *  @par 如meshttp协议每次通讯前都需要重新连接.
    */
    virtual void BeforeCommunication(){}
};

#endif // PROTOCOL_INTERFACE_H
