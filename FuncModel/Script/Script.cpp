#include "Script.h"
#include <QDebug>
#include <QString>
#include <QDir>
#include "MacroInterface.h"
#include "lua.hpp"

#include "IApp.h"

//外设模块，脚本接口使用
std::shared_ptr<interface_comm> Script::m_comm;

FuncModel *CreateFunc()
{
    return new Script();
}

Script::Script()
{
    m_L.lock = &m_lock;
}

Script::~Script()
{
    StopThread();
    //释放脚本对象
    foreach (auto obj, m_nameOfLuaobjMap) {
        delete  obj.second;
    }
    m_nameOfLuaobjMap.clear();
}

int Script::Init(std::shared_ptr<interface_comm> &comm)
{
    //1.配置功能参数检查
    m_parms.push_back("void(ScriptFunc,std::string)");
    m_parms.push_back("void(const std::string &,bool)");

    //2.初始化脚本功能
    m_comm = comm;
    InitLua();

    //3.加载脚本文件
    QDir dir2("project/script");
    QFileInfoList file_list = dir2.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    for (int j = 0; j < file_list.size(); ++j)
    {
        if(file_list.at(j).suffix() == "script")
        {
           LuaObject * object = new LuaObject(&m_L,this);
           if(object->InitFromFile(file_list.at(j).filePath().toLocal8Bit().data()))
           {//脚本加载成功
               if(1 == object->m_luaType)
               {//循环脚本
                   m_cycLua.push_back(object);
               }
               else if (0 == object->m_luaType)
               {//触发脚本,触发绑定
                   object->TriggerCallbackBind(m_comm.get());
               }
               else if (2 == object->m_luaType)
               {//开机脚本
                   m_bootLua.push_back(object);
               }
               //创建名称和脚本对象对应map
               m_nameOfLuaobjMap[file_list.at(j).fileName().split('.').first().toLocal8Bit().data()] = object;
           }
           else
           {
               delete object;
           }
        }
    }

    //4.初始化脚本线程
    InitThread();

    //5.执行开机脚本
    RunRootScript();

    return  0;
}

int Script::ExecFuncFromID(int funID, void **_a)
{
    int ret = 0;
    switch (funID)
    {
    case 0: { RegisterScriptFunc(Z_TRANSFORM(ScriptFunc,1),Z_TRANSFORM(std::string,2)); }  break;
    case 1: { RunScriptFromName(Z_TRANSFORM(const std::string,1),Z_TRANSFORM(bool,2)); }  break;
    default: ret = -1;
    }
    return ret;
}

const char *Script::CheckParms(int funID)
{
    if(static_cast<uint64_t>( funID ) >= m_parms.size())
    {
        return nullptr;
    }
    return m_parms[static_cast<uint32_t>( funID )].data();
}
int Script::GetFuncID()
{
    return 0x200 + 0xF326;
}

std::string Script::GetFuncName()
{
    return  "Script";
}

void Script::InsertTrigger(LuaObject *obj)
{
    std::lock_guard locker(m_triggerLock);
    m_triggerLua.push_back(obj);
}


int Script::InitLua()
{
    std::lock_guard locker(*(m_L.lock));
    if(!m_L.L && !m_initLuaFlag)
    {
        m_L.L =  luaL_newstate();

        luaL_openlibs(m_L.L);
        LuaFunc::Init();
        for (uint i = 0; i < LuaFunc::MFUNC_P.size() && i < LuaFunc::MFUNC_Name.size(); ++i) {
            lua_pushcfunction(m_L.L, LuaFunc::MFUNC_P.at(i));    // 将C函数转换为Lua的"function"并压入虚拟栈。
            lua_setglobal(m_L.L, LuaFunc::MFUNC_Name.at(i).data());// 弹出栈顶元素，并在Lua中用名为"mysin"的全局变量存储。
        }
        m_initLuaFlag = true;
    }
    return 0;
}

int Script::RunLua(const std::string &luaText)
{
    if(""== luaText) return 0;

    std::lock_guard locker(*(m_L.lock));
    if(m_L.L && lua_status(m_L.L) == LUA_OK)
    {
          if(luaL_dostring(m_L.L,luaText.data()))    // 执行Lua命令。
        {
            QString errStr = QString(QByteArray(lua_tostring(m_L.L,-1)));
            qDebug()<<errStr;
        }
    }
    else
    {
        qDebug()<<"The script runtime environment is abnormal.";
    }
    return 0;
}

int Script::RunLuaFromName(const std::string &name)
{
   std::map<std::string,LuaObject*>::iterator it =  m_nameOfLuaobjMap.find(name);
   if(it != m_nameOfLuaobjMap.end())
   {
       if(it->second->RunScript())
       {
           return 0;
       }
   }
   return -1;
}

void Script::InitThread()
{//初始化线程.
    m_bThreadFlag = true;
    m_processThread = std::make_shared<std::thread>(&Script::RunScript,this);
}

void Script::StopThread()
{
    if(m_processThread)
    {//关闭线程.
        m_bThreadFlag = false;
        m_processThread->join();
        m_processThread.reset();
    }
}

void Script::RunScript()
{
    while (m_bThreadFlag)
    {
        //外部执行脚本
        m_condLock.lock();
        foreach (auto luaobject, m_conditionLua) {
           RunLua(luaobject);
        }
        m_condLock.unlock();

        //已经触发的触发脚本
        m_triggerLock.lock();
        LuaObject * obj = nullptr;
        if(m_triggerLua.size() > 0)
        {
            obj = m_triggerLua.front();
            m_triggerLua.pop_front();
        }
        m_triggerLock.unlock();
        if(obj)
            obj->RunScript();

        //循环脚本
        foreach (auto luaobject, m_cycLua) {
           luaobject->RunScript();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Script::RunRootScript()
{
    foreach (auto script, m_bootLua) {
       script->RunScript();
    }
}

void Script::RegisterScriptFunc(ScriptFunc func, std::string name)
{
    m_extendScriptFunc[name] = func;

    std::lock_guard locker(*(m_L.lock));
    lua_pushcfunction(m_L.L,func);    //将C函数转换为Lua的"function"并压入虚拟栈。
    lua_setglobal(m_L.L,name.data()); //弹出栈顶元素，并在Lua中用名为name的全局变量存储。
}

void Script::RunScriptFromName(const std::string &name, bool isSync)
{
    if(isSync)
    {
        RunLuaFromName(name);
    }
    else
    {
        IAPP::GetIntance()->Submit([=](){
            this->RunLuaFromName(name);
        });
    }
}
