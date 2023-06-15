#include "checknetworkconnection.h"

#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtDebug>
#include <chrono>



CheckNetWorkConnection::CheckNetWorkConnection(QObject *parent) : QObject(parent)
{
    if(m_pMgr == nullptr)
        m_pMgr = new QNetworkConfigurationManager(this);
//    m_Mgr.allConfigurations(QNetworkConfiguration::Active);
    connect(m_pMgr, &QNetworkConfigurationManager::onlineStateChanged, [&,this](bool isconnect){
        setConnect(isconnect);
    });

    if(m_pTask == nullptr)
    {
        m_bTask = true;
        m_pTask.reset(new std::thread(&CheckNetWorkConnection::task, this));
    }

    checkNetWorkOnline();
}

CheckNetWorkConnection::~CheckNetWorkConnection()
{
    m_bTask = false;
    if(m_pTask == nullptr)
    {
        m_pTask->join();
        m_pTask = nullptr;
    }
}

bool CheckNetWorkConnection::isNetWorkOnline()
{
    QMutexLocker locker(&m_mutex);
    return m_bConnect;
}

void CheckNetWorkConnection::checkNetWorkOnline()
{
    QHostInfo::lookupHost("www.baidu.com", this, &CheckNetWorkConnection::onLookupHost);
}

void CheckNetWorkConnection::task()
{
    while(m_bTask)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));

        QMetaObject::invokeMethod(this, "checkNetWorkOnline", Qt::BlockingQueuedConnection);
//        checkNetWorkOnline();
    }
}

void CheckNetWorkConnection::setConnect(bool bConnect)
{
    bool bChanged = false;

    {
        QMutexLocker locker(&m_mutex);
        if(m_bConnect != bConnect)
        {
            bChanged = true;
            m_bConnect = bConnect;
        }
    }

    if(bChanged)
        emit signal_lookUpHostResult(bConnect);
}

void CheckNetWorkConnection::onLookupHost(const QHostInfo& host)
{
    if (host.error() == QHostInfo::NoError) {
        //网络连接，发送信号通知
        setConnect(true);
    }
    else{
        setConnect(false);
    }
}
