#include "ProtocolManager.h"
#include <QDir>
#include <QDebug>
#include "CommunicationManagement.h"
#include <glog/logging.h>

ProtocolManager::ProtocolManager()
{
}

ProtocolManager::~ProtocolManager()
{
    //释放协议插件线程.
    StopAllThread();

    //卸载
    UnInit();
}



void ProtocolManager::Init(const QList<LinkValue> &links)
{
    //读取支持的协议列表.
    m_supportLibNameMap.clear();
    QDir Dir("./CommunicationProtocol");
    QFileInfoList folder_list = Dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString slibName;
    for(int i = 0; i < folder_list.size(); i++)
    {
        slibName = folder_list.at(i).fileName();
        QDir dir2(folder_list.at(i).absoluteFilePath());
        QFileInfoList file_list = dir2.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        for (int j = 0; j < file_list.size(); ++j)
        {
            if(file_list.at(j).suffix() == "dll")
            {
                m_supportLibNameMap.insert(slibName+file_list.at(j).fileName().split('.').at(0),file_list.at(j).absoluteFilePath());
            }
        }
    }


    //开始加载.
    foreach (auto link, links)
    {
       QMap<QString,QString>::iterator it = m_supportLibNameMap.find(
                                            QString::fromStdString( link.Manufacturer+ link.Device_Name ));
       if(it != m_supportLibNameMap.end())
       {
           LoadProtocolFunc(it.value(),QString::fromStdString( link.Name ));
       }
       else if( CommunicationManagement::IsLinkHaveProtocol(link) )
       {//绑定的协议文件没找到.
          LOG(INFO)<<"Protocol plug-in not found:"<<link.Manufacturer+ link.Device_Name;
       }
    }

}

bool ProtocolManager::GetLinkFuncFromLinkName(const QString &linkName, Protocol::P_Protocol &p)
{
   QMap <QString,Protocol::P_Protocol>::iterator it = m_linkNameOfLibP.find(linkName);
   if(it == m_linkNameOfLibP.end())
   {
       return false;
   }
   else
   {
       p = it.value();
       return true;
   }
}

bool ProtocolManager::GetLoadProtocolListName(std::vector<std::string> &linkList)
{
    QStringList linkL = m_linkNameOfLibP.keys();
    foreach (auto link, linkL) {
        linkList.push_back(link.toStdString());
    }
    return true;
}

void ProtocolManager::StopAllThread()
{
    //释放协议插件线程.
    std::shared_ptr<ICommunication> comm;
    foreach (auto P, m_linkNameOfLibP) {
        P.InitDevice(&comm,nullptr);
    }
}

void ProtocolManager::UnInit()
{
    m_linkNameOfLibP.clear();
    m_supportLibNameMap.clear();
    //释放库.
    foreach (auto lib, m_libs) {
        lib->unload();
        lib->deleteLater();
    }
    m_libs.clear();
}


void ProtocolManager::LoadProtocolFunc(const QString &pathFileName,const QString &linkName)
{
    Protocol::P_Protocol protocolFuncP;
    QLibrary *pLib = new QLibrary(pathFileName);
    if (pLib->load())
    {
        protocolFuncP.Process =  reinterpret_cast<Protocol::P_Process>( pLib->resolve("Process") );
        protocolFuncP.GetValue =  reinterpret_cast<Protocol::P_GetValue>( pLib->resolve("GetValue") );
        protocolFuncP.SetValue =  reinterpret_cast<Protocol::P_SetValue>( pLib->resolve("SetValue") );
        protocolFuncP.CheckAddr =  reinterpret_cast<Protocol::P_CheckAddr>( pLib->resolve("CheckAddr") );
        protocolFuncP.GetRegInfo =  reinterpret_cast<Protocol::P_GetRegInfo>( pLib->resolve("GetRegInfo") );
        protocolFuncP.GetRegList =  reinterpret_cast<Protocol::P_GetRegList>( pLib->resolve("GetRegList") );
        protocolFuncP.GetVersion =  reinterpret_cast<Protocol::P_GetVersion>( pLib->resolve("GetVersion") );
        protocolFuncP.InitDevice =  reinterpret_cast<Protocol::P_InitDevice>( pLib->resolve("InitDevice") );
        protocolFuncP.SetTagList =  reinterpret_cast<Protocol::P_SetTagList>( pLib->resolve("SetTagList") );
        protocolFuncP.GetTagList =  reinterpret_cast<Protocol::P_GetTagList>( pLib->resolve("GetTagList") );
        protocolFuncP.GetDeviceConfig =  reinterpret_cast<Protocol::P_GetDeviceConfig>( pLib->resolve("GetDeviceConfig") );
        protocolFuncP.SetCycReadAddrinfo =  reinterpret_cast<Protocol::P_SetCycReadAddrinfo>( pLib->resolve("SetCycReadAddrinfo") );
        protocolFuncP.SetRegValueChangeCallback =  reinterpret_cast<Protocol::P_SetRegValueChangeCallback>( pLib->resolve("SetRegValueChangeCallback") );
    }
    else
    {//协议插件加载异常.
        LOG(INFO)<<linkName.toStdString()<<":"<<pLib->errorString().toStdString();
        pLib->deleteLater();
    }
    m_linkNameOfLibP.insert(linkName,protocolFuncP);
    m_libs.append(pLib);
}
