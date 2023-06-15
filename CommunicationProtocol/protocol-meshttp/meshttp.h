#ifndef mes_http_H
#define mes_http_H

#include "protocol_interface.h"
#include <nlohmann/json.hpp>

class mes_http :public Protocol_Interface
{
public:
    mes_http();
    virtual ~mes_http();

    //协议参数初始化.
    virtual void Init();

    //读.
    virtual int32_t ReadFrame(const std::vector<AddrInfo> &addrInfo);

    //写.
    virtual int32_t WriteFrame(const std::vector<Protocol::AddrInfoForRW> &addrInfo);

    //校验帧格式.
    virtual int32_t CheckFrame(Frame & recvF);

    //处理非标准数据解析特殊流程，return -1表示解析完成，上层不需要再次解析.
    virtual int32_t DealRecvData(AddrInfo &addrinfo, char * data);

    //每次通讯之前会调用.
    virtual void BeforeCommunication();

    //版本号.
    virtual int32_t GetVersion(std::string &sVersion);

private:
    Frame frame;
};

#endif // mes_http_H
