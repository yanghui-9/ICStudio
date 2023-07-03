#include "AppRun.h"
#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QStringLiteral>
#include <QDir>
#include <QLockFile>
#include <glog/logging.h>
#include <QIcon>

//用于捕捉异常
#include <Windows.h>
#include <string>
#include <DbgHelp.h>
#include <QDateTime>
#include <QMessageBox>
#pragma comment(lib, "DbgHelp.Lib")
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{//程式异常捕获
    //创建 Dump 文件
    QString filename = QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss-zzz") + ".dmp";
    HANDLE hDumpFile = CreateFile(filename.toStdWString().c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hDumpFile != INVALID_HANDLE_VALUE) {
        //Dump信息
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        //写入Dump文件内容
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, nullptr, nullptr);
    }
    //这里弹出一个错误对话框并退出程序
    EXCEPTION_RECORD* record = pException->ExceptionRecord;
    if(nullptr == record)
    {
        LOG(INFO)<<"EXCEPTION_RECORD* record = pException->ExceptionRecord; is empty point!";
        return -2;
    }
    QString errCode(QString::number(record->ExceptionCode, 16)), errAdr(QString::number(reinterpret_cast<uint>(record->ExceptionAddress), 16)), errMod;
    QMessageBox::critical(nullptr, QStringLiteral("程式崩溃"), QStringLiteral("软件发生了错误，现已保存了错误信息，并用于后续分析错误。") +
                          QStringLiteral("错误代码：%1错误地址：%2").arg(errCode).arg(errAdr),
                          QMessageBox::Ok);
    return EXCEPTION_EXECUTE_HANDLER;
}

static bool CheckSingleApp()
{
    //本测试程序id取名
    QString path = QDir::temp().absoluteFilePath(QString("%1.lock").arg(qApp->applicationName()));
    static QLockFile lockFile(path);
    if (!lockFile.tryLock(0))
    {
        return false;
    }
    return true;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //注冊异常捕获函数
    SetUnhandledExceptionFilter(ApplicationCrashHandler);

    //图标
    a.setWindowIcon(QIcon("://image/logo.ico"));

    //检查是否已经运行
    if (!CheckSingleApp())
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(QStringLiteral("程序已经在运行中\n")+
                       QStringLiteral("本软件只允许打开一次"));
        msgBox.exec();
        return 1;
    }

    //初始化
    AppRun app;
    app.InitRun();

    return a.exec();
}



