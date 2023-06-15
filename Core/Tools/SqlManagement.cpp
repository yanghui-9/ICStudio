#include "SqlManagement.h"
#include <QDebug>
#include <glog/logging.h>

SqlManagement* SqlManagement::m_instace = nullptr;
std::mutex m_War;
std::mutex u_War;

SqlManagement::SqlManagement(QObject *parent) : QObject(parent)
{
    database = QSqlDatabase::addDatabase("QSQLITE","MES");
    database.setDatabaseName(QCoreApplication::instance()->applicationDirPath() + "/MESLog.db");
    qDebug()  << QCoreApplication::instance()->applicationDirPath();
    if (!database.open()) {
        qDebug() << "MESLog.db open fail";
    }
    query = QSqlQuery(database);
    CleanSqlData();
    InitSql();
    QTimer* timer = new QTimer();
    if(timer == nullptr){
        qDebug() << "new QTimer failure";
    }
    connect(timer,&QTimer::timeout , this , &SqlManagement::Timing);
    timer->start(1000);
    WriteLogToDatabase();
}

SqlManagement::~SqlManagement()
{
    database.close();
    delete m_instace;
}

SqlManagement *SqlManagement::GetInstatce()
{
    std::lock_guard<std::mutex> lock(m_War);
    if(m_instace == nullptr){
        m_instace = new SqlManagement;
    }
    return m_instace;
}

QStringList SqlManagement::GetAllTableName(QString contains)
{
    QStringList tableslist = database.tables(QSql::Tables);
    QStringList list;
    if(contains == ""){
        return tableslist;
    }else{
        for (int i =0; i< tableslist.size();i++) {
            if(tableslist.at(i).contains(contains)) {
                list  << tableslist.at(i);
            }
        }
        return list;
    }
}

void SqlManagement::WriteLogToDatabase()
{
    std::thread thread_one([&,this]
    {
        while(1){

            LogData ld;
            if(sq.dequeue(ld)){
                InsertData(ld.type,ld.errNum, ld.logMsg, ld.time);
            }
        }
    });
    thread_one.detach();
}

std::vector<LogData> SqlManagement::GetLogInfo(std::string sTime, std::string eTime, const std::vector<std::string>& vType)
{
    std::lock_guard<std::mutex> lock(m_mutex1);
    std::vector<LogData> vData;
    QString condition ;
    for(int i = 0; i < vType.size(); i++)
    {
        if(i == 0)
            condition += QString(" and (");
        else
            condition += QString(" or");

        condition += QString(" LogType = '%1'").arg(stringToQString(vType.at(i)));

        if(i == vType.size()-1)
            condition += QString(")");
    }
    QStringList tableNamelist = GetAllTableName("LogD");
    for(int i = 0; i < tableNamelist.size();i++){
        QString sql = QString(u8"select LogType, ErrNum, LogMsg, DateTime from %1 where DateTime >= '%2' and DateTime <= '%3'")
                            .arg(tableNamelist.at(i)).arg(stringToQString(sTime)).arg(stringToQString(eTime)) + condition;
        bool flag = query.exec(sql);
        if(!flag){
//            LOG(ERROR) << "drop table is fail , sql is :" << query.lastQuery().toStdString() <<  "  err is " << query.lastError().text().toStdString();
        }
        while(query.next()) {
//            list << query.value(0).toString();
            LogData ldata;
            ldata.type = QStringToString(query.value(0).toString());
            ldata.errNum = QStringToString(query.value(1).toString());
            ldata.logMsg = QStringToString(query.value(2).toString());
            ldata.time = QStringToString(query.value(3).toString());
            vData.push_back(ldata);
        }
        query.clear();
    }
    return vData;
}


bool SqlManagement::InitSql()
{
    std::lock_guard<std::mutex> lock(m_mutex1);
    m_LogTableName = "LogD"+ QDate::currentDate().toString("yyyyMMdd");
    if (database.open())
    {
       QStringList sqllsit;
       QString create_sql;
       create_sql = QString("CREATE TABLE IF NOT EXISTS %1 (ID INTEGER  primary key AUTOINCREMENT, LogType varchar[10], ErrNum varchar[10], LogMsg varchar[100],"
                            "DateTime TimeStamp NOT NULL DEFAULT (datetime('now','localtime')))").arg(m_LogTableName);
       sqllsit << create_sql;
       sqllsit << create_sql;
       for(int i = 0; i< sqllsit.size();i++){
           if(!query.exec(sqllsit.at(i)))
           {
               qDebug() << "Error: Fail to create table." << query.lastError();
           }
       }
       query.clear();
    } else{
        return false;
    }

}

bool SqlManagement::CleanSqlData()
{
    std::lock_guard<std::mutex> lock(m_mutex1);
    bool flag = false;
    QString time = QDateTime::currentDateTime().addDays(-7).toString("yyyy-MM-dd") +QString(" 00:00:00");//获取系统现在的时间
    QString BeforeDaystr= QDateTime::currentDateTime().addDays(-7).toString("yyyyMMdd");//获取前一天时间
    QStringList tableNamelist = GetAllTableName();
    for(int i = 0; i < tableNamelist.size();i++){
        if(tableNamelist.at(i).contains("LogD")){
            QString data = tableNamelist.at(i).split("LogD").at(1);
            if(data.toInt() <= BeforeDaystr.toInt()){
                QString strSQL = QString("DROP TABLE %1").arg(tableNamelist.at(i));
                flag = query.exec(strSQL);
                if(!flag){
//                    LOG(ERROR) << "drop table is fail , sql is :" << query.lastQuery().toStdString() <<  "  err is " << query.lastError().text().toStdString();
                }
            }
        }
        query.clear();
    }
    return true;
}


QString SqlManagement::stringToQString(const std::string &sInfo)
{
     return QString::fromLocal8Bit(sInfo.c_str());
}

std::string SqlManagement::QStringToString(const QString &sInfo)
{
    return std::string(sInfo.toLocal8Bit().constData());
}

void SqlManagement::Timing()
{
    QDateTime current_time = QDateTime::currentDateTime();
    QString BeforeDaystr= QDateTime::currentDateTime().addDays(-1).toString("yyyy-MM-dd");//获取前一天时间
    QString CurrentDaystr= QDateTime::currentDateTime().toString("yyyy-MM-dd");//获取当前时间
    QString Now_time = current_time.toString("hh:mm:ss");
    if(Now_time == "00:00:00" || Now_time == "12:00:00"){
        CleanSqlData();
    }
}


void SqlManagement::Log(std::string type,std::string errNum, std::string logMsg , std::string time)
{
//    std::lock_guard<std::mutex> lock(m_mutex1);
    LogData ld;
    ld.type = type;
    ld.errNum = errNum;
    ld.logMsg = logMsg;
    ld.time = time;
    sq.enqueue(ld);

}

bool SqlManagement::InsertData(std::string type,std::string errNum, std::string logMsg, std::string time)
{
    std::lock_guard<std::mutex> lock(m_mutex1);
    query.prepare(QString("insert into %1( LogType, ErrNum, LogMsg) VALUES (:LogType, :ErrNum, :LogMsg) ").arg(m_LogTableName));
    query.bindValue(":LogType", stringToQString(type));
    query.bindValue(":ErrNum", stringToQString(errNum));
    query.bindValue(":LogMsg", stringToQString(logMsg));
    bool bExe = query.exec();
    if(!bExe) {
        qDebug() << "InsertData error!" << query.lastError().text();
    }
    query.clear();
    return bExe;

}

