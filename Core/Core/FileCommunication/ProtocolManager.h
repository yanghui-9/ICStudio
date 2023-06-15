#ifndef PROTOCOLMANAGER_H
#define PROTOCOLMANAGER_H
#include "../../CommunicationProtocol/common/protocol_type.h"
#include "../Interface/interface_comm.h"
#include <QMap>
#include <QLibrary>

class ProtocolManager
{
public:
    ProtocolManager();
    ~ProtocolManager();

    //初始化link协议库加载.
    void Init(const QList<LinkValue> &links );
    //通过link名称获取协议接口.
    bool GetLinkFuncFromLinkName(const QString &linkName,Protocol::P_Protocol &p);
    //获取加载的协议link名称列表.
    bool GetLoadProtocolListName(std::vector<std::string> &linkList);
    //关闭协议线程.
    void StopAllThread();
    //卸载.
    void UnInit();

protected:
    void LoadProtocolFunc(const QString &pathFileName, const QString &linkName);

private:
    QMap <QString,Protocol::P_Protocol> m_linkNameOfLibP;//通讯协议接口和连接名称
    QList <QLibrary*> m_libs;//协议库列表
    QMap<QString,QString> m_supportLibNameMap;//支持的协议插件列表
};

#endif // PROTOCOLMANAGER_H
