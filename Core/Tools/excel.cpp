#include "excel.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDateTime>

Excel::Excel(QString fileName, QObject *parent) : QObject(parent),
    m_BaseDir(QCoreApplication::applicationDirPath() + "/ExcelReport/")
  ,m_CustomFileName(fileName)

{

}

Excel::~Excel()
{
    if(m_file)
    {
        m_file->close();
        m_file->deleteLater();
        m_file = nullptr;
    }
}

int Excel::setHeaderTitle(const QStringList &titleList)
{
#if 0
    QXlsx::Document xlsx(m_filePathAndName);
    QXlsx::CellRange range = xlsx.dimension();
    if(0 == range.rowCount())
    {
        for(int i = 0 ; i < titleList.size() ; i++)
        {
            bool isWrite = xlsx.write(1,i+1,titleList.at(i));
        }
    }
    xlsx.saveAs(m_filePathAndName);
#endif
    if(!m_file)
    {
        m_file = new QFile(m_filePathAndName,this);
        if(!m_file->open(QIODevice::WriteOnly))
        {
            return -1;
        }
    }
    m_file->write((titleList.join(",")+"\n").toUtf8());

    return 0;
}

bool Excel::CreateExcelFile()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QString time = currentTime.toString("yyyy-MM-dd hh:mm:ss");
    QString date= currentTime.toString("yyyyMMdd");
    QString month = date.mid(4, 2);
    QString year=date.mid(0, 4);
    QString day = date.mid(6, 2);
    m_DirPath=m_BaseDir + year + "/" + month+"/" + day + "/";
    m_filePathAndName = m_DirPath + m_CustomFileName + "_" + date + ".csv";

    QDir dir;
    if (!(dir.exists(m_DirPath)))
    {
        dir.mkpath(m_DirPath);
    }
    QFileInfo fileInfo(m_filePathAndName);
    if(fileInfo.exists())
    {
        return true;
    }
    else
    {
        return  false;
    }

}

int Excel::insertData(QVector<QString> &data)
{
#if 0
    QXlsx::Document xlsx(m_filePathAndName);
    QXlsx::CellRange range = xlsx.dimension();
    int rowCount =  range.rowCount();
    for(int i = 0 ; i < data.size() ; i++)
    {
        bool isWrite = xlsx.write(rowCount+1,i+1,data.at(i));
        if(true != isWrite)
        {
            LOG(INFO)<<"insert EXCEL data false!";
        }
    }
    xlsx.saveAs(m_filePathAndName);
#endif

    if(!m_file)
    {
        m_file = new QFile(m_filePathAndName,this);
        if(!m_file->open(QIODevice::WriteOnly))
        {
            return -1;
        }
    }
    m_file->write((data.toList().join(",")+"\n").toUtf8());

    return 0;
}
