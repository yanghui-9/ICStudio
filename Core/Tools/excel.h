#ifndef EXCEL_H
#define EXCEL_H

#include <QObject>
#include <QDir>
#include <QTimer>
#include "tools.h"

class _TOOLS Excel : public QObject
{
    Q_OBJECT
public:
    explicit Excel(QString fileName , QObject *parent = nullptr);
    virtual ~Excel();
    int setHeaderTitle(const QStringList &titleList);
    bool CreateExcelFile();
    int insertData(QVector<QString> &data);
    QString getExcelFileName(){return m_filePathAndName;}

signals:
    void New_day();

private:
    QString m_BaseDir;
    QString m_DirPath;
    QString m_filePathAndName;
    QString m_CustomFileName;
    QFile   *m_file = nullptr;
};

#endif // EXCEL_H
