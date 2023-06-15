#ifndef cip_H
#define cip_H

#include "../common/protocol_interface.h"
#include <nlohmann/json.hpp>


class cip :public Protocol_Interface
{
public:
    cip();
    virtual ~cip(){}

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

    //注册帧-握手帧.
    virtual void GetGlobalFrame();

protected:
    //组建帧头.
    void FormFrameHead(std::vector<char> &send, uint32_t sendLen, char fun);
    //根据数据类型计算长度.
    uint32_t GetLenFromDataType(unsigned short datatype);
    //根据数据类型获取命令码
    char GetCommandFromDataType(Protocol::Protocol_DataType dataType);
    //单个标签读.
    void OneTagReadFrame(const AddrInfo &addrInfo);
    //更新字符串标签的长度.
    void SetTagOfStringLen(const std::string &name,uint64_t len);
    //重新计算帧长部分
    void RecalculateFrameLen(Frame &curframe);

private:
    Frame frame;
    bool m_bHandshake = false;
    //plc会话句柄.
    char m_SessionHandle[4] = {0};
};

#endif // cip_H
