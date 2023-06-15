#include "test1.h"
#include "IApp.h"
#include "extend_func_def.h"
#include "extend_func_head.h"
#include "lua.hpp"


FuncModel *CreateFunc()
{
    return new test1();
}


int test1::Init(std::shared_ptr<interface_comm> &comm)
{
    Q_UNUSED(comm);
    IAPP::GetIntance()->InvokeMethod(Extend_Script_Register,Z_ARG(ScriptFunc,test1::l_test1),Z_ARG(std::string,"test0425"));
    //IAPP::GetIntance()->InvokeMethod(Extend_Script_Register,Z_ARG(ScriptFunc,test1::l_test1));
    //IAPP::GetIntance()->InvokeMethod(Extend_Script_Register,Z_ARG(ScriptFunc,test1::l_test1),Z_ARG(std::string,1));
    return  0;
}

#include <QDebug>
int test1::l_test1(lua_State *L)
{
    int type = lua_type(L,1);
    switch (type) {
    case LUA_TSTRING:{qDebug()<<lua_tostring(L,1);}break;
    case LUA_TNIL:{qDebug()<<"nil";}break;
    case LUA_TBOOLEAN:{qDebug()<<lua_toboolean(L,1);}break;
    case LUA_TNUMBER:{qDebug()<<lua_tonumber(L,1);}break;
    }
    return 0;
}

