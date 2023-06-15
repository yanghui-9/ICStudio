#ifndef COMMUNICATIONMANAGEMENT_H
#define COMMUNICATIONMANAGEMENT_H
#include <set>
#include <mutex>
#include <shared_mutex>
#include "../Interface/interface_comm.h"
#include "ProtocolManager.h"

class CommunicationManagement :public interface_comm
{
    friend class ProtocolManager;
public:
    struct LinkPackage
    {
        LinkValue linkV;//通讯参数
        std::shared_ptr<ICommunication> comm;//通讯对象
        ushort commStatus = 0;//通讯连接状态（无通讯协议link使用）
    };

    CommunicationManagement();
    virtual ~CommunicationManagement();

    //初始化所有link.
    virtual int Init();
    //卸载所有link.
    virtual int UnInit();

    //关闭启动指定link.
    virtual int CloseLink(const std::string &linkName);
    virtual int StartLink(const std::string &linkName);

    //动态获取link配置.
    virtual int GetLinkConfig(const std::string &linkName,LinkValue &linkV);
    virtual int SetLinkConfig(const std::string &linkName,const LinkValue &linkV);

    ////绑定了地址协议的link使用.
    virtual int GetValue(Protocol::AddrInfoForRW &addr,const Protocol::Read_Opt_Type &RType);
    virtual int SetValue(Protocol::AddrInfoForRW &addr, const Protocol::Write_Opt_Type &WType);
    virtual int SetCycReadAddrinfo(const std::string &linkName,
                                   const std::vector<Protocol::AddrInfoForRW> &addrVec,
                                   Protocol::Add_Type addType);
    virtual int SetAddrValueChangeCB(const std::string &linkName,
                                     const std::vector<Protocol::AddrInfoForRW> &addrVec,
                                     Protocol::Add_Type addType);
    //标签操作.
    virtual int32_t GetTagList(const std::string &linkName,std::vector<Protocol::Tag_Info> &tagList);
    virtual int32_t SetTagList(const std::string &linkName,const std::vector<Protocol::Tag_Info> &tagList);

    //通讯连接未绑定协议可以使用通讯接口.
    virtual int WriteAndRead(const std::string &linkName,char *sendBuf,int len,
                              const ResponseCallBack &cb,int timeout, int recvLenMax);
    virtual int  Write(const std::string &linkName,char *buf,int len);
    virtual int  Read(const std::string &linkName,char *buf,int len);
    //Tcp服务端可注册事件回调来通讯.
    virtual int  SetCallback(const std::string &linkName,const ConnectCB &connectCB = nullptr,
                             const DisconnectCB &disconnectCB = nullptr,
                             const ReadReadyCB &ReadReadyCB = nullptr);

protected:
    bool readFile(QJsonDocument& outDoc, const QString& fileName);

    //获取该link有没有绑定通讯协议.
    static bool IsLinkHaveProtocol(const LinkValue &linkV);

private:
    std::map <std::string,LinkPackage> m_linkNameOfObjMap;//link对应的属性包.

    std::vector<std::string> m_linkNameOfLibList;//绑定了协议的link名称列表.

    ProtocolManager m_protocolManager;//协议管理.

    std::shared_mutex m_mutex;
};

#endif // COMMUNICATIONMANAGEMENT_H
