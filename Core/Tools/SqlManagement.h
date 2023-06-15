#ifndef SQLMANAGEMENT_H
#define SQLMANAGEMENT_H

#include <QObject>
//#include <QFileDialog>
#include <QDateTime>
#include <QDate>
#include <QTimer>
#include <mutex>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
//#include <QFileDialog>
#include <QDateTime>
#include <QDate>
#include <QSqlRecord>
#include <QSqlField>
#include "tools.h"
#include<QCoreApplication>
#include "../../communication/SafeQueue.h"



struct LogData{
    std::string type;       //日志类型： 操作日志 外设交互日志 异常日志
    std::string errNum;     //错误码： 没有时为空，或者一个通用的
    std::string logMsg;     //日志信息
    std::string time;       //日志产生的时间
};


#define get_Database SqlManagement::GetInstatce()

class _TOOLS  SqlManagement : public QObject
{
    Q_OBJECT
public:
    explicit SqlManagement(QObject *parent = nullptr);

    ~SqlManagement();

    static SqlManagement *GetInstatce();

    // sTime:开始时间(格式：2023-01-01 00:00:00) ， eTime：结束时间 ， type: 日志类型，没有赋值时默认查询全部
    std::vector<LogData> GetLogInfo(std::string sTime, std::string eTime, const std::vector<std::string>& vType = std::vector<std::string>());
    // type: 日志类型 ， errNum：错误码 ， logMsg： 日志内容 time： 时间 （格式： 2023-01-01 00:00:00）
    void Log(std::string type,std::string errNum, std::string logMsg , std::string time);

private:
    bool InitSql();                     // 初始化
    bool CreatNewTable();               // 分表,定时调用
    bool CleanSqlData();                // 清理数据,定时调用

    bool InsertData(std::string type,std::string errNum, std::string logMsg, std::string time);
    QString stringToQString(const std::string& sInfo);
    std::string QStringToString(const QString& sInfo);

    QSqlDatabase GetDataBase() { return database ;}                             // 获取数据库对象
    QString GetLogTableName() {return m_LogTableName;}          // 获取交互日志表名
    QStringList GetAllTableName(QString contains = "");                         // 获取所有表格

    void WriteLogToDatabase();


private slots:
    void Timing();

private:
    QString m_LogTableName;
    QSqlDatabase database;
    QString m_shift;
    static SqlManagement* m_instace;
    std::mutex m_mutex1;
    int num = 0;
    bool preState = false;
    QSqlQuery query;
    SafeQueue<LogData> sq;
};

#endif // SQLMANAGEMENT_H
