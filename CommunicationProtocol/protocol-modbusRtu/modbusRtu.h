#ifndef modbusRtu_H
#define modbusRtu_H

#include "../common/protocol_interface.h"


class modbusRtu :public Protocol_Interface
{
public:
    modbusRtu();
    virtual ~modbusRtu(){}

    //协议参数初始化.
    virtual void Init();

    //读.
    virtual int32_t ReadFrame(const std::vector<AddrInfo> &addrInfo);

    //写.
    virtual int32_t WriteFrame(const std::vector<Protocol::AddrInfoForRW> &addrInfo);

    //校验帧格式.
    virtual int32_t CheckFrame(Frame & recvF);

    //版本号.
    virtual int32_t GetVersion(std::string &sVersion);


private:
    Frame frame;
    bool m_bHandshake = false;
    //plc会话句柄.
    char m_SessionHandle[4] = {0};
};

#endif // modbusRtu_H
