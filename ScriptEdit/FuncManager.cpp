#include "FuncManager.h"
#include <QDir>
#include <QDebug>
#include <QSettings>

FuncManager::FuncManager()
{
}

FuncManager::~FuncManager()
{
    //foreach (auto lib, m_libs) {
    //    lib->unload();
    //    lib->deleteLater();
    //}
}

void FuncManager::InitLoad(std::vector<std::shared_ptr<FuncModel>> &modelVector)
{
    //读配置文件获取待加载模块名.
    QSettings sysini("project/system.ini",QSettings::IniFormat);
    QStringList sFuncNameList = sysini.value("backstage").toString().split(',');

    //读取支持的列表.
    m_supportLibNameMap.clear();
    QDir dir2("./Func");
    QFileInfoList file_list = dir2.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    for (int j = 0; j < file_list.size(); ++j)
    {
        if(file_list.at(j).suffix() == "dll")
        {
            m_supportLibNameMap.insert(file_list.at(j).fileName().split('.').at(0),file_list.at(j).absoluteFilePath());
        }
    }

    //开始加载.
    foreach (auto name, sFuncNameList) {
        QMap<QString,QString>::iterator it = m_supportLibNameMap.find( name );
        if(it != m_supportLibNameMap.end())
        {
            LoadProtocolFunc(it.value());
        }
        else
        {
           //LOG(INFO)<<"Background plug-in not found:"<<name.toStdString();
        }
        if(m_pFunc)
        {
            modelVector.push_back( std::shared_ptr<FuncModel>( m_pFunc() ) );
        }
    }
}

void FuncManager::LoadProtocolFunc(const QString &pathFileName)
{
    m_pFunc = nullptr;
    QLibrary *pLib = new QLibrary(pathFileName);
    if (pLib->load())
    {
        m_pFunc =  reinterpret_cast<P_CreateFunc>( pLib->resolve("CreateFunc") );
    }
    else
    {
        //LOG(INFO)<<pathFileName.toStdString()<<":"<<pLib->errorString().toStdString();
        pLib->deleteLater();
    }
    m_libs.append(pLib);
}
