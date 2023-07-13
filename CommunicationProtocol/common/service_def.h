#ifndef SERVICE_DEF_H
#define SERVICE_DEF_H
#include "protocol_type.h"

#define ADDRESS_AREA_UNIT (256)
#define RECV_BUFF_MAX     (10240)

//地址信息（读写组帧使用）.
struct AddrInfo
{
    std::string reg;     //地址模式下-寄存器，标签模式下-标签名称.
    uint64_t index = 0;  //索引(位).
    uint64_t len = 1;    //位的长度.

    uint64_t beginIndex = 0;//解析数据部分开始索引.(字节单位)
    uint64_t dataLen = 0;   //解析数据长度(字节)
    uint16_t bitOffset = 0;//位偏移
    std::vector<Protocol::AddrInfoForRW> varList; //读的变量信息列表.

    AddrInfo(const std::string &_reg,uint64_t _index,uint64_t _len):reg(_reg),index(_index),len(_len){}
    AddrInfo(){}

    bool operator == (const AddrInfo & info) const
    {
        if(reg == info.reg && index == info.index && len == info.len)
        {
            return true;
        }
        return false;
    }
    bool operator < (const AddrInfo & info) const
    {
        if(reg < info.reg)
        {
            return true;
        }
        else if (reg == info.reg) {
           if(index < info.index)
           {
               return true;
           }
           else if (index == info.index) {
               if(len < info.len)
               {
                   return true;
               }
           }
        }
        return false;
    }
};

//收发帧结构.
struct Frame
{
    std::vector<char> send;
    std::vector<char> recv;

    uint32_t sendLen = 0;//发送长度.
    int32_t recvLen = 0;//是预计接收长度.
    int32_t actualRecvLen = 0;//实际接收长度.
    bool bGlobalFrame = false;//标识是不是全局帧，比如握手帧，全局帧不会放入帧缓存列表.

    std::vector<AddrInfo> readList;//读取地址列表(包含读的变量列表以及回调).
    std::vector<Protocol::AddrInfoForRW> writeList;//写入变量信息.

    char * GetSendBuff(int len = -1){
        if(-1 != len)
        {
            send.resize(static_cast<uint32_t>(len));
        }
        if(0 != len)
            return const_cast<char *>( send.data() );
        else
            return nullptr;
    }
    char * GetRecvBuff(int len = -1){
        if(-1 != len)
        {
            recv.resize(static_cast<uint32_t>(len));
        }
        if(0 != len)
        {
            return const_cast<char *>( recv.data() );
        }
        else {
            return nullptr;
        }
    }
};

struct Data_Area
{
    std::string reg;
    uint64_t    index;
    char        data[ADDRESS_AREA_UNIT] = {0};
    //std::vector<char> data;
    bool operator < (const Data_Area & info) const
    {
        int ret = strcmp(reg.data(),info.reg.data());
        if(0 > ret){
            return true;
        }else if (0 == ret) {
            //if( abs( static_cast<int64_t>(info.index) - static_cast<int64_t>(index) ) >= (ADDRESS_AREA_UNIT*8) )
            if( index <info.index )
            {
                return true;
            }
        }
        return false;
    }
};
#endif // SERVICE_DEF_H
