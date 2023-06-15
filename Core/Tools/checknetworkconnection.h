#ifndef CHECKNETWORKCONNECTION_H
#define CHECKNETWORKCONNECTION_H

#include <QObject>
#include <QMutex>
#include <QtNetwork/QHostInfo>
#include <thread>
#include "tools.h"



class QNetworkConfigurationManager;
using namespace std;

class _TOOLS CheckNetWorkConnection : public QObject
{
    Q_OBJECT
public:
    explicit CheckNetWorkConnection(QObject *parent = nullptr);
    virtual ~CheckNetWorkConnection();

public:
    //判断网络是否有连接
    bool isNetWorkOnline();

    //判断是否能上网
    Q_INVOKABLE void checkNetWorkOnline();

private:
    void task();
    void setConnect(bool bConnect);

private slots:
    void onLookupHost(const QHostInfo& host);

signals:
    void signal_lookUpHostResult(bool);

private:
    QNetworkConfigurationManager* m_pMgr = nullptr;

    bool m_bTask = false;
    std::shared_ptr<std::thread> m_pTask = nullptr;

    QMutex m_mutex;
    bool m_bConnect = true;
};

#endif // CHECKNETWORKCONNECTION_H
