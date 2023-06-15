#include "LuaObject.h"
#include "MacroInterface.h"
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include "lua.hpp"
#include "Script.h"

LuaObject::LuaObject(ULua_State *L,Script *script):m_L(L),m_ScriptMdeol(script)
{
}

bool LuaObject::InitFromFile(const std::string &file)
{
    QFile dFile(QString::fromLocal8Bit(file.data()));
    if(dFile.open(QIODevice::ReadOnly))
    {
        QDataStream dataS(&dFile);
        //字体属性
        QString fontFamily;
        dataS >> fontFamily;
        int fontsize;
        dataS >> fontsize;
        //触发类型
        dataS >>m_luaType;
        //触发数据
        int triggerVarType;//触发变量模式
        dataS >> triggerVarType;
        QString linkName;//连接名称
        dataS >> linkName;
        QString reg;//寄存器名称
        dataS >> reg;
        QString sIndex;//索引字符串
        dataS >> sIndex;
        uint64_t Index;//索引
        dataS >> Index;
        int dataType;//数据类型
        dataS >> dataType;
        //循环时间
        dataS >> m_cycTime;
        //脚本内容
        QString text;;
        dataS >> text;

        dFile.close();

        //数据初始化到界面
        m_triggerVar.linkName = linkName.toLocal8Bit().data();
        m_triggerVar.reg = reg.toLocal8Bit().data();
        m_triggerVar.sIndex = sIndex.toLocal8Bit().data();
        m_triggerVar.index = Index;
        m_triggerVar.dataType = static_cast<Protocol::Protocol_DataType>( dataType );
        m_triggerVar.callback = std::bind(&LuaObject::TriggerCB,this,std::placeholders::_1,std::placeholders::_2);

        //处理脚本内容
        return InitLuaText(m_L->L,text.toLocal8Bit().data(),m_runBuff);
    }
    return false;
}

bool LuaObject::RunScript()
{
    if(0 == m_runBuff.size())
        return false;

    std::lock_guard locker(*(m_L->lock));
    lua_load(m_L->L,LuaFunc::myReader,&m_runBuff,"Test",nullptr);
    int type = lua_type(m_L->L,-1);
    if(type != LUA_TFUNCTION)//字符串错误信息
    {
        if(type == LUA_TSTRING)
        {
            QString errStr(lua_tostring(m_L->L,-1));
            qDebug()<<"loadLuaError:"<<errStr;
        }
        lua_pop(m_L->L,1);
        return true;
    }
    int runFlag = lua_pcall(m_L->L, 0, LUA_MULTRET, 0);
    if(runFlag)
    {
        QString errStr = QString(QByteArray(lua_tostring(m_L->L,-1)));
        qDebug()<<QString("errorcod = %1,callLuaError:%2").arg(runFlag).arg(errStr);
        lua_pop(m_L->L,1);
        return true;
    }
    //qDebug()<<"num:"<<lua_gettop(m_L->L);
    //lua_pop(myL,1);
    return false;
}

bool LuaObject::TriggerCallbackBind(interface_comm * comm)
{
    if(0 != m_luaType || !comm)
    {
        return false;
    }
    std::vector<Protocol::AddrInfoForRW > cbList;
    cbList.push_back(m_triggerVar);
    comm->SetAddrValueChangeCB(m_triggerVar.linkName,cbList,Protocol::Append);
    return true;
}

bool LuaObject::InitLuaText(lua_State *L,const std::string &text, std::vector<char> &buff)
{
    int flagLoad = luaL_loadstring(L,text.data());
    if(flagLoad != 0)
    {
        return false;
    }
    if(lua_dump(L,LuaFunc::myWriter,&buff,1)!= 0)
    {
        QString errStr(lua_tostring(L,-1));
        qDebug()<<"LuaTextInitError:"<<errStr;
    }
    lua_pop(L,1);
    //qDebug()<<"lua_len3:"<<lua_gettop(L_cyc)<<"type:"<<lua_type(L_cyc,-1);
    return true;
}

void LuaObject::TriggerCB(const Protocol::AddrInfoForRW &addr, int32_t code)
{
    Q_UNUSED(addr);
    Q_UNUSED(code);
    if(m_ScriptMdeol)
    {
        m_ScriptMdeol->InsertTrigger(this);
    }
}
