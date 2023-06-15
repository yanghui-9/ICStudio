#include "UISupportManager.h"
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <glog/logging.h>


UISupportManager::UISupportManager()
{
}

UISupportManager::~UISupportManager()
{
    //foreach (auto lib, m_libs) {
    //    lib->unload();
    //    lib->deleteLater();
    //}
}

IUiModel *UISupportManager::InitLoad()
{
    //读取配置
    QSettings sysini("project/system.ini",QSettings::IniFormat);
    QString sFuncName = sysini.value("ui").toString();
    if(sFuncName.isNull())
    {
        return nullptr;
    }

    //读取支持的插件
    m_supportLibNameMap.clear();
    QDir dir2("./UiSupport");
    QFileInfoList file_list = dir2.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    for (int j = 0; j < file_list.size(); ++j)
    {
        if(file_list.at(j).suffix() == "dll")
        {
            m_supportLibNameMap.insert(file_list.at(j).fileName().split('.').at(0),file_list.at(j).absoluteFilePath());
        }
    }

    //开始加载.
    QMap<QString,QString>::iterator it = m_supportLibNameMap.find( sFuncName );
    if(it != m_supportLibNameMap.end())
    {
        LoadProtocolFunc(it.value());
    }
    else
    {
        LOG(INFO)<<"Front end plug-in not found:"<<sFuncName.toStdString();
    }
    if(m_pFunc)
    {
        return  m_pFunc();
    }
    return nullptr;
}

void UISupportManager::LoadProtocolFunc(const QString &pathFileName)
{
    QLibrary *pLib = new QLibrary(pathFileName);
    if (pLib->load())
    {
        m_pFunc =  reinterpret_cast<P_CreateUiM>( pLib->resolve("CreateUiM") );
    }
    else
    {
        LOG(INFO)<<pathFileName.toStdString()<<":"<<pLib->errorString().toStdString();
        pLib->deleteLater();
    }
    m_libs.append(pLib);
}
