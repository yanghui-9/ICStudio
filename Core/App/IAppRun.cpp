#include "IAppRun.h"
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <glog/logging.h>
#include "event_def.h"
#include "../Tools/tools.h"

Y_EXPORT IAPP * IAPP::Instance = nullptr;

//配置输出日志的目录
#define LOGDIR "log"

IAppR::IAppR(const int n_threads):
    m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false)
{
    Instance = this;

    //检查日志缓存目录
    CheckCache();

    //初始化线程池
    Init();


    //初始化glog
    QDir dir;
    if (!dir.exists("./log"))
    {
        dir.mkpath("./log");
    }
    google::InitGoogleLogging("MesHost");
    google::SetStderrLogging(google::GLOG_INFO); //设置级别高于 google::INFO 的日志同时输出到屏幕
    FLAGS_colorlogtostderr = true;    //设置输出到屏幕的日志显示相应颜色
    FLAGS_log_dir = "./log";
    google::SetLogDestination(google::GLOG_INFO,LOGDIR"/INFO_"); //设置 google::INFO 级别的日志存储路径和文件名前缀
    google::SetLogDestination(google::GLOG_WARNING,LOGDIR"/WARNING_");   //设置 google::WARNING 级别的日志存储路径和文件名前缀
    google::SetLogDestination(google::GLOG_ERROR,LOGDIR"/ERROR_");   //设置 google::ERROR 级别的日志存储路径和文件名前缀
    FLAGS_logbufsecs =0;        //缓冲日志输出，默认为30秒，此处改为立即输出
    FLAGS_max_log_size =100;  //最大日志大小为 100MB
    FLAGS_stop_logging_if_full_disk = true;     //当磁盘被写满时，停止日志输出

    //FLAGS_logtostderr = true;	//是否打印到控制台
    //FLAGS_alsologtostderr = true;
}

IAppR::~IAppR()
{
    //释放线程池
    shutdown();
    //glog关闭
    google::ShutdownGoogleLogging();
}

void IAppR::AddTask(std::function<void ()> &task, Event_Pri_Type type)
{
    // Enqueue generic wrapper function
    switch (type) {
    case Event_Hi:{ m_queueHi.enqueue(task); }break;
    case Event_Nor:{ m_queue.enqueue(task); }break;
    case Event_Low:{ m_queueLo.enqueue(task); }break;
    }

    // Wake up one thread if its waiting
    m_conditional_lock.notify_one();
}

void IAppR::Init()
{
    for (uint i = 0; i < m_threads.size(); ++i) {
        m_threads[i] = std::thread(ThreadWorker(this));
    }
}

void IAppR::shutdown()
{
    m_shutdown = true;
    m_conditional_lock.notify_all();
    for (uint i = 0; i < m_threads.size(); ++i) {
        if(m_threads[i].joinable()) {
            m_threads[i].join();
        }
    }
}

void IAppR::PostEvent(HEvent *event, Event_Pri_Type type)
{
    Submit(type,std::bind(&IAppR::PostEventDeal,this,event));
}

void IAppR::SendEvent(HEvent *event)
{
    Event(event);
}

void GetContentsInParentheses(std::string &str)
{
    int index0 =  str.find('(');
    int index1 =  str.find(')');
    if(-1 != index0 && -1 != index1)
    {
        str = str.substr(index0,index1);
    }
}

bool IAppR::InvokeMethod(uint ID, Func_Call_Type callType, ZGenericReturnArgument ret,
                         ZGenericArgument val0,
                         ZGenericArgument val1,
                         ZGenericArgument val2,
                         ZGenericArgument val3,
                         ZGenericArgument val4,
                         ZGenericArgument val5)
{
    bool bRet = true;
    ZObject * model =  GetFuncModelFromID(ID>>8);
    if(model)
    {
        QVarLengthArray<char, 512> sig;
        const char *typeNames[] = {
            ret.name(),
            val0.name(),
            val1.name(),
            val2.name(),
            val3.name(),
            val4.name(),
            val5.name()
        };
        //返回类型
        uint len = qstrlen(typeNames[0]);
        if (len > 0)
        {
            sig.append(typeNames[0], static_cast<int>(len));
        }
        sig.append('(');
        //参数处理
        int paramCount;
        for (paramCount = 1; paramCount < ZMaximumParamCount; ++paramCount) {
            len = qstrlen(typeNames[paramCount]);
            if (len <= 0)
                break;
            sig.append(typeNames[paramCount], static_cast<int>(len));
            sig.append(',');
        }
        if (paramCount == 1)
            sig.append(')'); // no parameters
        else
            sig[sig.size() - 1] = ')';
        sig.append('\0');
        //检查参数是否正确
        const char * tmpParms =  model->CheckParms(ID%(0x100));
        if(tmpParms)
        {
            if(0 == qstrcmp(sig.constData(),tmpParms))
            {
                // invoke!
                if(Func_Call_Sync ==  callType)
                {//同步调用
                    void *param[] = {
                        ret.data(),
                        val0.data(),
                        val1.data(),
                        val2.data(),
                        val3.data(),
                        val4.data(),
                        val5.data()
                    };
                    model->ExecFuncFromID(ID%(0x100),param);
                }
                else
                {//异步调用
                    Submit([ID,model,ret,val0,val1,val2,val3,val4,val5](){
                        void *param[] = {
                            ret.data(),
                            val0.data(),
                            val1.data(),
                            val2.data(),
                            val3.data(),
                            val4.data(),
                            val5.data()
                        };
                        model->ExecFuncFromID(ID%(0x100),param);});
                }
            }
            else
            {
                bRet = false;
                qWarning("invokeMethod: No such method %s::%s  %s",
                         model->GetFuncName().data(), sig.constData(), tmpParms);
            }
        }
        else
        {
            bRet = false;
            qWarning("invokeMethod: ID No such method %s",sig.constData());
        }
    }
    return bRet;
}

bool IAppR::InvokeMethod(uint ID, Func_Call_Type callType,
                         ZGenericArgument val0,
                         ZGenericArgument val1,
                         ZGenericArgument val2,
                         ZGenericArgument val3,
                         ZGenericArgument val4,
                         ZGenericArgument val5)
{
    bool bRet = true;
    ZObject * model =  GetFuncModelFromID(ID>>8);
    if(model)
    {
        QVarLengthArray<char, 512> sig;
        const char *typeNames[] = {
            val0.name(),
            val1.name(),
            val2.name(),
            val3.name(),
            val4.name(),
            val5.name()
        };
        sig.append('(');
        //参数处理
        uint len;
        int paramCount;
        for (paramCount = 0; paramCount < ZMaximumParamCount-1; ++paramCount) {
            len = qstrlen(typeNames[paramCount]);
            if (len <= 0)
                break;
            sig.append(typeNames[paramCount], static_cast<int>(len));
            sig.append(',');
        }
        if (0 == paramCount)
            sig.append(')'); // no parameters
        else
            sig[sig.size() - 1] = ')';
        sig.append('\0');
        //检查参数是否正确
        const char * tmParms =  model->CheckParms(ID%(0x100));
        std::string tmpParms = "";
        if(tmParms)
        {
          tmpParms = tmParms;
        }
        GetContentsInParentheses(tmpParms);
        if(!tmpParms.empty() && 0 == qstrcmp(sig.constData(),tmpParms.data()))
        {
            if(0 == strcmp(sig.constData(),tmpParms.data()))
            {
               // invoke!
                if(Func_Call_Sync ==  callType)
                {//同步调用
                    void *param[] = {
                        nullptr,
                        val0.data(),
                        val1.data(),
                        val2.data(),
                        val3.data(),
                        val4.data(),
                        val5.data()
                    };
                    model->ExecFuncFromID(ID%(0x100),param);
                }
                else
                {//异步调用
                    Submit([ID,model,val0,val1,val2,val3,val4,val5](){
                        void *param[] = {
                            nullptr,
                            val0.data(),
                            val1.data(),
                            val2.data(),
                            val3.data(),
                            val4.data(),
                            val5.data()
                        };
                        model->ExecFuncFromID(ID%(0x100),param);});
                }
            }
            else
            {
                bRet = false;
                qWarning("invokeMethod: No such method %s::%s%s",
                         model->GetFuncName().data(), sig.constData(), tmpParms.data());
            }
        }
        else
        {
            bRet = false;
            qWarning("invokeMethod: ID No such method %s",sig.constData());
        }
    }
    return bRet;
}

void IAppR::PostEventDeal(HEvent *event)
{
    SendEvent(event);
    delete event;
}

void IAppR::CheckCache()
{
    //1.检查日志文件目录大小是否超过限制
    //读取日志目录最大容量
    uint64_t logMaxSize = 100;//M
    // 打开json文件.
    QFile jsonFIle("system/systemConfig.json");
    if( jsonFIle.open(QIODevice::ReadWrite))
    {
        QJsonParseError jsonParserError;
        QJsonDocument outDoc = QJsonDocument::fromJson(jsonFIle.readAll(),&jsonParserError);
        jsonFIle.close();
        // 文档不空 && jsonParserError解析没有错误，解析成功返回true.
        if(!outDoc.isNull() && (jsonParserError.error == QJsonParseError::NoError))
        {
            QJsonObject objRoot =  outDoc.object();
            logMaxSize =  static_cast<uint64_t>(objRoot["systemParams"].toObject().value("iLogDirMaxSize").toInt(100));
        }
    }
    std::string logPath = "log";
    if( Tools_GetDirSize(&logPath) > logMaxSize )
    {//超过最大容量删除
        //QDir qdir("log");
        //qdir.removeRecursively();//未知原因，在部分电脑上导致删除的文件夹无法进入，不可访问
        QDir dir("log");
        if(dir.exists())
        {
            dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
            QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
            foreach (QFileInfo file, fileList){ //遍历文件信息
                if (file.isFile()){ // 是文件，删除
                    file.dir().remove(file.fileName());
                }
            }
        }
    }
}

