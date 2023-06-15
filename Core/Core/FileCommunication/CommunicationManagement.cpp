#include "CommunicationManagement.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QFile>
#include <QJsonArray>

#include "../../communication/communicationdef.h"
#include "../../communication/communicationglobal.h"

CommunicationManagement::CommunicationManagement()
{
}

CommunicationManagement::~CommunicationManagement()
{
    UnInit();
}

int CommunicationManagement::Init()
{
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    //加载通讯配置.
    QJsonDocument configDoc;
    if(readFile(configDoc,"project/communication.d"))
    {
        if(configDoc.isArray())
        {//link array
            QJsonArray linkArray =  configDoc.array();
            foreach (auto link, linkArray) {
                LinkPackage linkP;
                QJsonObject linkobj =  link.toObject();
                linkP.linkV.Name =  linkobj["Name"].toString().toStdString();
                linkP.linkV.Manufacturer = linkobj["Manufacturer"].toString().toStdString();
                linkP.linkV.Device_Name = linkobj["Device_Name"].toString().toStdString();

                QJsonObject comInfoObj =  linkobj["CommInfo"].toObject();
                linkP.linkV.config.commInfo.commType = static_cast<Protocol::Comm_Type>( comInfoObj["commType"].toInt() );
                linkP.linkV.config.commInfo.sCommPar =  comInfoObj["sCommPar"].toString().toStdString();
                linkP.linkV.config.commInfo.isThread = comInfoObj["isThread"].toBool();
                linkP.linkV.config.commInfo.isOnline = comInfoObj["isOnline"].toBool();
                linkP.linkV.config.commInfo.isOpenLog = comInfoObj["isOpenLog"].toBool();

                QJsonObject Device_ConfigObj =  linkobj["Device_Config"].toObject();
                linkP.linkV.config.protocolInfo.station = Device_ConfigObj["station"].toInt();
                linkP.linkV.config.protocolInfo.timeout = static_cast<uint32_t>( Device_ConfigObj["timeout"].toInt() );
                linkP.linkV.config.protocolInfo.retryTimes = static_cast<uint32_t>(Device_ConfigObj["retryTimes"].toInt());
                linkP.linkV.config.protocolInfo.RWThreshold = Device_ConfigObj["RWThreshold"].toInt();
                linkP.linkV.config.protocolInfo.byteOrder16 = Device_ConfigObj["byteOrder16"].toInt();
                linkP.linkV.config.protocolInfo.byteOrder32 = Device_ConfigObj["byteOrder32"].toInt();
                linkP.linkV.config.protocolInfo.bitPackageMax = static_cast<uint32_t>(Device_ConfigObj["bitPackageMax"].toInt());
                linkP.linkV.config.protocolInfo.deviceStation = Device_ConfigObj["deviceStation"].toInt();
                linkP.linkV.config.protocolInfo.masterOrSlave = Device_ConfigObj["masterOrSlave"].toInt();
                linkP.linkV.config.protocolInfo.wordPackageMax = static_cast<uint32_t>(Device_ConfigObj["wordPackageMax"].toInt());
                linkP.linkV.config.protocolInfo.bitPackageInterval = static_cast<uint32_t>(Device_ConfigObj["bitPackageInterval"].toInt());
                linkP.linkV.config.protocolInfo.wordPackageInterval = static_cast<uint32_t>(Device_ConfigObj["wordPackageInterval"].toInt());
                linkP.linkV.config.protocolInfo.communicationInterval = static_cast<uint32_t>(Device_ConfigObj["communicationInterval"].toInt());
                linkP.linkV.config.protocolInfo.nonstandardParameters = Device_ConfigObj["nonstandardParameters"].toString().toStdString();

                //初始化通讯接口.
                std::shared_ptr<ICommunication> ptrComm(CreateComm(static_cast<ICommunication::Communication_Type>( linkP.linkV.config.commInfo.commType ),
                                                                   linkP.linkV.config.commInfo.sCommPar));
                linkP.comm = ptrComm;
                if(!IsLinkHaveProtocol(linkP.linkV))
                {//未绑定通讯协议
                   linkP.commStatus = static_cast<ushort>( linkP.comm->OpenComm() );
                }

                //插入.
                m_linkNameOfObjMap.insert(std::pair<std::string,LinkPackage>(linkP.linkV.Name,std::move( linkP )));
            }
        }
    }

    //初始协议加载.
    QList<LinkValue> links;
    foreach (auto link, m_linkNameOfObjMap) {
        links.append(link.second.linkV);
    }
    m_protocolManager.Init(links);
    m_protocolManager.GetLoadProtocolListName(m_linkNameOfLibList);

    //协议启动
    foreach (auto link, m_linkNameOfObjMap)
    {
        Protocol::P_Protocol P;
        m_protocolManager.GetLinkFuncFromLinkName(QString::fromStdString( link.second.linkV.Name ),P);
        if(P.InitDevice){
            P.InitDevice(&link.second.comm,&link.second.linkV.config);
        }
    }
    return 0;
}

int CommunicationManagement::UnInit()
{
    //std::unique_lock<std::shared_mutex> lock(m_mutex);
    //关闭协议线程
    m_protocolManager.StopAllThread();

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    //清除协议数据
    m_linkNameOfObjMap.clear();
    m_linkNameOfLibList.clear();

    //协议管理卸载
    m_protocolManager.UnInit();

    return -1;
}

int CommunicationManagement::Read(const std::string &linkName, char *buf, int len)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    std::map <std::string,LinkPackage>::iterator it =  m_linkNameOfObjMap.find(linkName);
    if(it != m_linkNameOfObjMap.end())
    {
        return  it->second.comm->RecvData(buf,len,it->second.linkV.config.protocolInfo.timeout);
    }
    return -1;
}

int CommunicationManagement::SetCallback(const std::string &linkName,const interface_comm::ConnectCB &connectCB,
                                         const interface_comm::DisconnectCB &disconnectCB,
                                         const interface_comm::ReadReadyCB &ReadReadyCB)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    std::map <std::string,LinkPackage>::iterator it =  m_linkNameOfObjMap.find(linkName);
    if(it != m_linkNameOfObjMap.end())
    {
        return  it->second.comm->OpenComm(0,500,connectCB,disconnectCB,ReadReadyCB);
    }
    return -1;
}

int CommunicationManagement::Write(const std::string &linkName, char *buf, int len)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    std::map <std::string,LinkPackage>::iterator it =  m_linkNameOfObjMap.find(linkName);
    if(it != m_linkNameOfObjMap.end())
    {
        return  it->second.comm->SendData(buf,static_cast<uint32_t>( len ));
    }
    return -1;
}

int CommunicationManagement::WriteAndRead(const std::string &linkName, char *sendBuf, int len,
                                           const ResponseCallBack &cb, int timeout, int recvLenMax)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    std::map <std::string,LinkPackage>::iterator it =  m_linkNameOfObjMap.find(linkName);
    if(it != m_linkNameOfObjMap.end())
    {
        return it->second.comm->AsynWR(sendBuf,len,cb,recvLenMax,timeout);
    }
    return -1;
}

int CommunicationManagement::SetAddrValueChangeCB(const std::string &linkName, const std::vector<Protocol::AddrInfoForRW> &addrVec,
                                                  Protocol::Add_Type addType)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    //过滤.
    std::vector<Protocol::AddrInfoForRW> addrinfoVec;
    foreach (auto addr, addrVec) {
        if(addr.linkName == linkName)
        {
           addrinfoVec.emplace_back(addr);
        }
    }
    //设置到对应的协议中生效.
    Protocol::P_Protocol P;
    if(m_protocolManager.GetLinkFuncFromLinkName(QString::fromStdString( linkName ),P))
    {
        if( P.SetCycReadAddrinfo )
        {
            P.SetRegValueChangeCallback(&addrinfoVec,addType);
        }
        return 0;
    }
    return -1;
}

int32_t CommunicationManagement::GetTagList(const std::string &linkName, std::vector<Protocol::Tag_Info> &tagList)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    Protocol::P_Protocol P;
    if(m_protocolManager.GetLinkFuncFromLinkName(QString::fromStdString( linkName ),P))
    {
        P.GetTagList(&tagList);
        return 0;
    }
    return -1;
}

int32_t CommunicationManagement::SetTagList(const std::string &linkName, const std::vector<Protocol::Tag_Info> &tagList)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    Protocol::P_Protocol P;
    if(m_protocolManager.GetLinkFuncFromLinkName(QString::fromStdString( linkName ),P))
    {
        P.SetTagList(&tagList);
        return 0;
    }
    return -1;
}

int CommunicationManagement::SetCycReadAddrinfo(const std::string &linkName, const std::vector<Protocol::AddrInfoForRW> &addrVec,
                                                Protocol::Add_Type addType)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    //过滤.
    std::vector<Protocol::AddrInfoForRW> addrinfoVec;
    foreach (auto addr, addrVec) {
        if(addr.linkName == linkName)
        {
           addrinfoVec.emplace_back(addr);
        }
    }
    //设置到对应的协议中生效.
    Protocol::P_Protocol P;
    if(m_protocolManager.GetLinkFuncFromLinkName(QString::fromStdString( linkName ),P))
    {
        if( P.SetCycReadAddrinfo )
        {
            P.SetCycReadAddrinfo(&addrinfoVec,addType);
        }
        return 0;
    }
    return -1;
}

int CommunicationManagement::SetValue(Protocol::AddrInfoForRW &addr, const Protocol::Write_Opt_Type &WType)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    Protocol::P_Protocol P;
    foreach (auto linkName, m_linkNameOfLibList) {
        if(addr.linkName  == linkName &&
                m_protocolManager.GetLinkFuncFromLinkName(QString::fromStdString( linkName ),P))
        {
            lock.unlock();
            return P.SetValue(&addr,WType);
        }
    }
    return -1;
}

int CommunicationManagement::GetValue(Protocol::AddrInfoForRW &addr, const Protocol::Read_Opt_Type &RType)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    Protocol::P_Protocol P;
    foreach (auto linkName, m_linkNameOfLibList) {
        if(addr.linkName  == linkName &&
                m_protocolManager.GetLinkFuncFromLinkName(QString::fromStdString( linkName ),P))
        {
            lock.unlock();
            return P.GetValue(&addr,RType);
        }
    }

    if(Protocol_Status_Reg_Name == addr.reg && Protocol::CurrentValue == RType)
    {//获取未绑定通讯协议link的通讯状态.
        std::map <std::string,LinkPackage>::iterator it = m_linkNameOfObjMap.find(addr.linkName);
        if(it != m_linkNameOfObjMap.end() &&
           !IsLinkHaveProtocol(it->second.linkV))
        {
            addr.value = it->second.commStatus;
            return 0;
        }
    }

    return -1;
}

int CommunicationManagement::SetLinkConfig(const std::string &linkName,const LinkValue &linkV)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    std::map <std::string,LinkPackage>::iterator it =  m_linkNameOfObjMap.find( linkName );
    if(it != m_linkNameOfObjMap.end())
    {
        it->second.linkV = linkV;
        return 0;
    }
    //同步.
    Protocol::P_Protocol P;
    if(m_protocolManager.GetLinkFuncFromLinkName(QString::fromStdString( linkName ),P))
    {
        std::shared_ptr<ICommunication> comm;
        P.InitDevice(&comm,&linkV.config);
        return 0;
    }
    return  -1;
}

int CommunicationManagement::GetLinkConfig(const std::string &linkName,LinkValue &linkV)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    std::map <std::string,LinkPackage>::iterator it =  m_linkNameOfObjMap.find( linkName );
    if(it != m_linkNameOfObjMap.end())
    {
        linkV = it->second.linkV;
        return 0;
    }
    return  -1;
}

int CommunicationManagement::StartLink(const std::string &linkName)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    Protocol::P_Protocol P;
    if(m_protocolManager.GetLinkFuncFromLinkName(QString::fromStdString( linkName ),P))
    {//绑定了通讯协议的直接操作协议接口.
        Protocol::Device_Config config;
        P.GetDeviceConfig(&config);
        config.commInfo.isOnline= true;
        std::shared_ptr<ICommunication> comm;
        P.InitDevice(&comm,&config);
        return 0;
    }

    std::map <std::string,LinkPackage>::iterator it = m_linkNameOfObjMap.find(linkName);
    if(it != m_linkNameOfObjMap.end() &&
            !IsLinkHaveProtocol(it->second.linkV))
    {//未绑定通讯协议的直接操作通讯接口.
        it->second.comm->OpenComm();
        return 0;
    }


    return -1;
}

int CommunicationManagement::CloseLink(const std::string &linkName)
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    Protocol::P_Protocol P;
    if(m_protocolManager.GetLinkFuncFromLinkName(QString::fromStdString( linkName ),P))
    {//绑定了通讯协议的直接操作协议接口.
        Protocol::Device_Config config;
        P.GetDeviceConfig(&config);
        config.commInfo.isOnline = false;
        std::shared_ptr<ICommunication> comm;
        P.InitDevice(&comm,&config);
        return 0;
    }

    std::map <std::string,LinkPackage>::iterator it = m_linkNameOfObjMap.find(linkName);
    if(it != m_linkNameOfObjMap.end() &&
            !IsLinkHaveProtocol(it->second.linkV))
    {//未绑定通讯协议的直接操作通讯接口.
        it->second.comm->CloseComm();
        return 0;
    }

    return -1;
}



bool CommunicationManagement::readFile(QJsonDocument& outDoc, const QString& fileName)
{
    // 打开json文件.
    QFile jsonFIle( fileName );
    if( !jsonFIle.open(QIODevice::ReadWrite))
    {
        qDebug() << "open file jsonObject.json error!";
        return false;
    }
    QJsonParseError jsonParserError;
    outDoc = QJsonDocument::fromJson(jsonFIle.readAll(),&jsonParserError);
    jsonFIle.close();
    // 文档不空 && jsonParserError解析没有错误，解析成功返回true.
    return (!outDoc.isNull() && (jsonParserError.error == QJsonParseError::NoError));
}

bool CommunicationManagement::IsLinkHaveProtocol(const LinkValue &linkV)
{
    if(linkV.Device_Name.empty() && linkV.Manufacturer.empty())
    {
        return false;
    }
    return true;
}
