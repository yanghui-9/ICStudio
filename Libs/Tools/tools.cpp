#include "tools.h"
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QCryptographicHash>
#include <QSettings>
#include <QTextCodec>
#include <QFileInfoList>


//判断压缩文件是否加密
bool JudgePassWord(const std::string &sFileName)
{
    QProcess myProcess;
    QStringList args;
    args.append("l");
    args.append(QString::fromUtf8(sFileName.data()));
    myProcess.start("./7z.exe",args);
    myProcess.waitForFinished();
    QString sReturn = QString::fromUtf8( myProcess.readAllStandardOutput() );
    if(sReturn.contains("7zAES"))
    {
        return true;
    }
    return false;
}

//计算文件md5
QString FileMd5(const QStringList &pathList,int IType = 0)
{
    QCryptographicHash::Algorithm alg ;
    if(0 == IType)
    {
        alg = QCryptographicHash::Md5;
    }
    else
    {
        alg = QCryptographicHash::Sha256;
    }
    QCryptographicHash hash(alg);
    foreach (auto file, pathList) {
        QFile sourceFile(file);
        if (sourceFile.open(QIODevice::ReadOnly)) {
            hash.addData(sourceFile.readAll());
            sourceFile.close();
        }
    }
    return hash.result().toHex();
}

//检查project目录下的配置文件是否改动，改动的话，版本号加1
//参数 iType 0-导出时，非0导入时
bool JudgeChangeMd5(int iType = 0)
{
    QDir Dir("project");
    QStringList fileList;
    QString versionFile;
    if(Dir.exists())
    {
       QFileInfoList infoList =  Dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
       foreach (auto file, infoList) {
           if(file.fileName().contains("version.ini"))
           {
               versionFile = file.absoluteFilePath();
               continue;
           }
           fileList.append(file.absoluteFilePath());
       }
       if(0 < fileList.size() && QFile::exists(versionFile))
       {
           QSettings versionIni(versionFile, QSettings::IniFormat);
           QString DM5 =  FileMd5(fileList);
           if( DM5 != versionIni.value("MD5").toString() )
           {//MD5不一致修改过配置内容
              //子版本号加一，更新MD5值
               if(0 == iType)
               {//导出时
                   versionIni.setValue("MD5",DM5);
                   versionIni.setValue("versionS",versionIni.value("versionS").toInt()+1);
                   versionIni.sync();
               }
               else
               {//导入时
                   versionIni.setValue("MD5",DM5);
               }
               return true;
           }
           if(0 != iType)
           {//导入时
               //更新sha256
               QFile WSha256("protect.exe");
               if(WSha256.open(QIODevice::WriteOnly))
               {
                 WSha256.write( FileMd5(fileList,256).toLocal8Bit() );
               }
           }
       }
    }
    return false;
}

bool Tools_ExportProject(const std::string &sFileName)
{
    //处理配置文件版本号
    if(JudgeChangeMd5())
    {
        //qDebug()<<"config change!!!";
    }
    else
    {
        //qDebug()<<"config no change!!!";
    }

    //先删除文件
    QFile::remove(QString::fromStdString( sFileName ));

    //开始导出
    QProcess myProcess;
    QStringList args;
    args.append("a");
    args.append(QString::fromUtf8(sFileName.data()));
    args.append(".\\project");
    args.append("-r");
    args.append("-y");
    args.append("-pvF9GGQ/QHA]F]<](2{?[r}}A.t-S8w");
    myProcess.start("./7z.exe",args);
    myProcess.waitForFinished();
    QString sReturn = QString::fromUtf8( myProcess.readAllStandardOutput() );
    if(sReturn.contains("cannot open") || sReturn.contains("Errors:"))
    {
        return false;
    }
    //qDebug()<<sReturn;
    return true;
}


bool Tools_ImportProject(const std::string &sFileName)
{
    bool ret = false;
    if(!JudgePassWord(sFileName))
    {//没加密
        qDebug()<<"no password!!!!!";
        return false;
    }
    QProcess myProcess;
    QStringList args;
    args.append("x");
    args.append(QString::fromUtf8(sFileName.data()));
    args.append("-o.\\");
    args.append("-r");
    args.append("-y");
    args.append("-pvF9GGQ/QHA]F]<](2{?[r}}A.t-S8w");
    myProcess.start("./7z.exe",args);
    myProcess.waitForFinished();
    QString sReturn = QString::fromUtf8( myProcess.readAllStandardOutput() );
    //qDebug()<<sReturn;
    if(sReturn.contains("Can't open as archive") || sReturn.contains("Errors:"))
    {
       ret = false;
    }
    else
    {
       ret = true;
    }

    //处理md5和sha256
    JudgeChangeMd5(256);

    return ret;
}

bool Tools_CheckProject()
{
    QDir Dir("project");
    QStringList fileList;
    QString versionFile;
    if(Dir.exists())
    {
       QFileInfoList infoList =  Dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
       foreach (auto file, infoList) {
           if(file.fileName().contains("version.ini"))
           {
               versionFile = file.absoluteFilePath();
               continue;
           }
           fileList.append(file.absoluteFilePath());
       }
       if(0 < fileList.size() && QFile::exists(versionFile))
       {
           //先检查MD5
           QSettings versionIni(versionFile, QSettings::IniFormat);
           QString DM5 =  FileMd5(fileList);
           if( DM5 != versionIni.value("MD5").toString() )
           {//MD5不一致修改过配置内容
               return false;
           }
           //再检查sha256
           QFile protect("protect.exe");
           if(protect.open(QIODevice::ReadOnly))
           {
               if(FileMd5(fileList,256) != protect.readAll())
               {
                   return false;
               }
           }
       }
    }
    return true;
}

void Tools_GBK2UTF8(std::string *inStr)
{
    QTextCodec *gbk = QTextCodec::codecForName("gbk");
    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QString str = gbk->toUnicode((*inStr).data());
    *inStr = utf8->fromUnicode( gbk->toUnicode((*inStr).data()) ).data();
}


uint64_t GetDirSize(const QString &filePath)
{
    QDir tmpDir(filePath);
    uint64_t size = 0;

    /*获取文件列表  统计文件大小*/
    foreach(QFileInfo fileInfo, tmpDir.entryInfoList(QDir::Files))
    {
        size += fileInfo.size();
    }

    /*获取文件夹  并且过滤掉.和..文件夹 统计各个文件夹的文件大小 */
    foreach(QString subDir, tmpDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        size += GetDirSize(filePath + QDir::separator() + subDir); //递归进行统计所有子目录
    }

    return size/1024/1024;
}

uint64_t Tools_GetDirSize(std::string *sdirPath)
{
    QString sPath = QString::fromLocal8Bit(sdirPath->data());
    return GetDirSize(sPath);
}


/*********************************************************************/
  /*功能：拷贝文件夹
    qCopyDirectory -- 拷贝目录
    fromDir : 源目录
    toDir   : 目标目录
    bCoverIfFileExists : ture:同名时覆盖  false:同名时返回false,终止拷贝
    返回: ture拷贝成功 false:拷贝未完成*/
/***********************************************************************/
bool Tools_CopyDirectory(const std::string& fromDir, const std::string& toDir, bool bCoverIfFileExists)
{
    QDir formDir_(QString::fromLocal8Bit( fromDir.data() ));
    QDir toDir_(QString::fromLocal8Bit( toDir.data() ));

    if(!toDir_.exists())
    {
        if(!toDir_.mkdir(toDir_.absolutePath()))
            return false;
    }

    QFileInfoList fileInfoList = formDir_.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        //拷贝子目录
        if(fileInfo.isDir())
        {
            //递归调用拷贝
            if(!Tools_CopyDirectory(fileInfo.filePath().toStdString(), toDir_.filePath(fileInfo.fileName()).toStdString(),true))
                return false;
        }
        //拷贝子文件
        else
        {
            if(bCoverIfFileExists && toDir_.exists(fileInfo.fileName()))
            {
                toDir_.remove(fileInfo.fileName());
            }
            if(!QFile::copy(fileInfo.filePath(), toDir_.filePath(fileInfo.fileName())))
            {
                return false;
            }
        }
    }
    return true;
}
