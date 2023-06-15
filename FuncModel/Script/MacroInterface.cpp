#include "MacroInterface.h"
#include <QTextCodec>
#include "lua.hpp"
#include "../../CommunicationProtocol/common/protocol_type.h"
#include "Script.h"

std::vector<ScriptFunc> LuaFunc::MFUNC_P;
std::vector<std::string> LuaFunc::MFUNC_Name;

void LuaFunc::Init()
{
   MFUNC_P.push_back(LuaFunc::l_SetValue);
   MFUNC_P.push_back(LuaFunc::l_GetValue);
   MFUNC_P.push_back(LuaFunc::l_SendData);
   MFUNC_P.push_back(LuaFunc::l_RecvData);
   MFUNC_P.push_back(LuaFunc::l_Gbk2Utf);
   MFUNC_P.push_back(LuaFunc::l_Utf2Gbk);
   MFUNC_P.push_back(LuaFunc::l_Log);

   MFUNC_Name.push_back("SetValue");
   MFUNC_Name.push_back("GetValue");
   MFUNC_Name.push_back("SendData");
   MFUNC_Name.push_back("RecvData");
   MFUNC_Name.push_back("Gbk2Utf");
   MFUNC_Name.push_back("Utf2Gbk");
   MFUNC_Name.push_back("LOG");
}


int LuaFunc::l_Gbk2Utf(lua_State *L)
{
    int type = lua_type(L,1);
    int len = (int)lua_tonumber(L,2);
    if(len > 1024)
        len = 1024;
    if(type == 5)//table
    {
        char data[1024] = {0};
        for(int i = 0;i <len%1024 ; i++)
        {
            lua_rawgeti(L,1,i+1);
            data[i] = lua_tointeger(L,-1);
            lua_pop(L,1);
        }
        QTextCodec *gbk =QTextCodec::codecForName("gbk");
        QTextCodec *utf8= QTextCodec::codecForName("UTF-8");
        QByteArray arr=utf8->fromUnicode(gbk->toUnicode(data,len));
        for(int n=0;n<arr.size();n++)
        {
            lua_pushinteger(L,(lua_Integer)((uchar)arr.at(n)));        //将数组的数据入栈
            lua_rawseti(L,1,n+1);        //将刚刚入栈的数据设置为数组的第n+1个数据，同时这个数据会自动从栈里pop
        }
        lua_pushnumber(L,len);
        return 1;
    }
    lua_pushnumber(L,0);
    return 1;
}

int LuaFunc::l_Utf2Gbk(lua_State *L)
{
    int type = lua_type(L,1);
    int len = (int)lua_tonumber(L,2);
    if(len > 1024)
        len = 1024;
    if(type == 5)//table
    {
        char data[1024] = {0};
        for(int i = 0;i <len%1024 ; i++)
        {
            lua_rawgeti(L,1,i+1);
            data[i] = lua_tointeger(L,-1);
            lua_pop(L,1);
        }
        QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
        QTextCodec *gbk = QTextCodec::codecForName("GBK");
        QByteArray arr=gbk->fromUnicode(utf8->toUnicode(data,len));
        for(int n=0;n<arr.size();n++)
        {
            lua_pushinteger(L,(lua_Integer)((uchar)arr.at(n)));        //将数组的数据入栈
            lua_rawseti(L,1,n+1);        //将刚刚入栈的数据设置为数组的第n+1个数据，同时这个数据会自动从栈里pop
        }
        lua_pushnumber(L,arr.size());
        return 1;
    }
    lua_pushnumber(L,0);
    return 1;
}

int LuaFunc::l_Log(lua_State *L)
{
    int type = lua_type(L,1);
    switch (type) {
    case LUA_TSTRING:{LOG(INFO)<<LOG_HREAD<<lua_tostring(L,1);}break;
    case LUA_TNIL:{LOG(INFO)<<LOG_HREAD<<"nil";}break;
    case LUA_TBOOLEAN:{LOG(INFO)<<LOG_HREAD<<lua_toboolean(L,1);}break;
    case LUA_TNUMBER:{LOG(INFO)<<LOG_HREAD<<lua_tonumber(L,1);}break;
    }
    return 0;
}

bool DealValueFromDataType(Protocol::AddrInfoForRW &addr,lua_State *L,int index)
{
    bool bRet = false;
    switch (addr.dataType)
    {
    case Protocol::bit:
    {
        if(LUA_TBOOLEAN == lua_type(L,index))
        {
           addr.value = lua_toboolean(L,index);
           bRet = true;
        }
    }
        break;
    case Protocol::bit8:
    {
        if(LUA_TNUMBER == lua_type(L,index))
        {
           addr.value = static_cast<char>(lua_tonumber(L,index)+0.5);
           bRet = true;
        }
    }
        break;
    case Protocol::ubit8:
    {
        if(LUA_TNUMBER == lua_type(L,index))
        {
           addr.value = static_cast<uchar>(lua_tonumber(L,index)+0.5);
           bRet = true;
        }
    }
        break;
    case Protocol::bit16:
    {
        if(LUA_TNUMBER == lua_type(L,index))
        {
           addr.value = static_cast<short>(lua_tonumber(L,index)+0.5);
           bRet = true;
        }
    }
        break;
    case Protocol::ubit16:
    {
        if(LUA_TNUMBER == lua_type(L,index))
        {
           addr.value = static_cast<ushort>(lua_tonumber(L,index)+0.5);
           bRet = true;
        }
    }
        break;
    case Protocol::bit32:
    {
        if(LUA_TNUMBER == lua_type(L,index))
        {
           addr.value = static_cast<int32_t>(lua_tonumber(L,index)+0.5);
           bRet = true;
        }
    }
        break;
    case Protocol::ubit32:
    {
        if(LUA_TNUMBER == lua_type(L,index))
        {
           addr.value = static_cast<uint32_t>(lua_tonumber(L,index)+0.5);
           bRet = true;
        }
    }
        break;
    case Protocol::float32:
    {
        if(LUA_TNUMBER == lua_type(L,index))
        {
           addr.value = static_cast<float>(lua_tonumber(L,index));
           bRet = true;
        }
    }
        break;
    case Protocol::bit64:
    {
        if(LUA_TNUMBER == lua_type(L,index))
        {
           addr.value = static_cast<int64_t>(lua_tonumber(L,index)+0.5);
           bRet = true;
        }
    }
        break;
    case Protocol::ubit64:
    {
        if(LUA_TNUMBER == lua_type(L,index))
        {
           addr.value = static_cast<uint64_t>(lua_tonumber(L,index)+0.5);
           bRet = true;
        }
    }
        break;
    case Protocol::double64:
    {
        if(LUA_TNUMBER == lua_type(L,index))
        {
           addr.value = lua_tonumber(L,index);
           bRet = true;
        }
    }
        break;
    case Protocol::string:
    {
        if(LUA_TSTRING == lua_type(L,index))
        {
           addr.value = lua_tostring(L,index);
           bRet = true;
        }
    }
        break;
    default:{}
    }
    return bRet;
}

bool DealValueFromDataTypeOfTolua(Protocol::AddrInfoForRW &addr,lua_State *L,int index)
{
    if(0 == addr.value.index())
    {
        return false;
    }
    switch (addr.dataType)
    {
    case Protocol::bit:{lua_pushboolean(L,std::get<bool>(addr.value));}break;
    case Protocol::bit8:{lua_pushinteger(L,std::get<char>(addr.value));}break;
    case Protocol::ubit8:{lua_pushinteger(L,std::get<uchar>(addr.value));}break;
    case Protocol::bit16:{lua_pushinteger(L,std::get<short>(addr.value));}break;
    case Protocol::ubit16:{lua_pushinteger(L,std::get<ushort>(addr.value));}break;
    case Protocol::bit32:{lua_pushinteger(L,std::get<int32_t>(addr.value));}break;
    case Protocol::ubit32:{lua_pushinteger(L,std::get<uint32_t>(addr.value));}break;
    case Protocol::float32:{lua_pushnumber(L,std::get<float>(addr.value));}break;
    case Protocol::double64:{lua_pushnumber(L,std::get<double>(addr.value));}break;
    case Protocol::ubit64:{lua_pushinteger(L,std::get<uint64_t>(addr.value));}break;
    case Protocol::bit64:{lua_pushinteger(L,std::get<int64_t>(addr.value));}break;
    default:{}
    }
    lua_rawseti(L,index,1);
    return true;
}

int LuaFunc::l_SetValue(lua_State *L)
{
   int ret = 0;
   Protocol::AddrInfoForRW addr;
   if(LUA_TSTRING == lua_type(L,1))
   {//linkname
       addr.linkName =  lua_tostring(L,1);
   }
   else
   {
      ret = -1;
   }
   if(LUA_TSTRING == lua_type(L,2))
   {//reg
       addr.reg =  lua_tostring(L,2);
   }
   else
   {
      ret = -1;
   }
   if(LUA_TNUMBER == lua_type(L,3))
   {//index
       addr.index =  static_cast<uint64_t>( lua_tonumber(L,3)+0.5 );
   }
   else
   {
      ret = -1;
   }
   if(LUA_TNUMBER == lua_type(L,4))
   {//datatype
       addr.dataType =  static_cast<Protocol::Protocol_DataType>( (int)(lua_tonumber(L,4)+0.5) );
   }
   else
   {
      ret = -1;
   }
   //数据转换
   if(!DealValueFromDataType(addr,L,5))
   {
      ret = -1;
   }
   //检查以上参数是否正常
   if(0 == ret)
   {
       //同步写入
       if(Script::m_comm)
       {
           ret = Script::m_comm->SetValue(addr,Protocol::SyncWrite);
       }
   }
   //返回值
   lua_pushinteger(L,ret);
   return 1;
}

int LuaFunc::l_GetValue(lua_State *L)
{
    int ret = 0;
    Protocol::AddrInfoForRW addr;
    if(LUA_TSTRING == lua_type(L,1))
    {//linkname
        addr.linkName =  lua_tostring(L,1);
    }else
    {
       ret = -1;
    }
    if(LUA_TSTRING == lua_type(L,2))
    {//reg
        addr.reg =  lua_tostring(L,2);
    }else
    {
       ret = -1;
    }
    if(LUA_TNUMBER == lua_type(L,3))
    {//index
        addr.index =  static_cast<uint64_t>( lua_tonumber(L,3)+0.5 );
    }else
    {
       ret = -1;
    }
    if(LUA_TNUMBER == lua_type(L,4))
    {//datatype
        addr.dataType =  static_cast<Protocol::Protocol_DataType>( (int)(lua_tonumber(L,4)+0.5) );
    }else
    {
       ret = -1;
    }
    //同步写入
    if(0 == ret)
    {
        if(Script::m_comm)
        {
            ret = Script::m_comm->GetValue(addr,Protocol::SyncRead);
            //结果返回（参数）
            if(!DealValueFromDataTypeOfTolua(addr,L,5))
            {
                ret = -1;
            }
        }
    }
    //返回值
    lua_pushinteger(L,ret);
    return 1;
}

int LuaFunc::l_SendData(lua_State *L)
{
    return 0;
}

int LuaFunc::l_RecvData(lua_State *L)
{
    return 0;
}

int LuaFunc::myWriter(lua_State *L, const void *p, size_t sz, void *ud)
{
    Q_UNUSED(L);
    std::vector<char> *luabuf = reinterpret_cast<std::vector<char> *>(ud);
    uint64_t buffSize = luabuf->size();
    luabuf->resize(buffSize + sz);
    memcpy(luabuf->data()+buffSize,p,sz);
    return 0;
}

const char * LuaFunc::myReader(lua_State *L, void *ud, size_t *sz)
{
    Q_UNUSED(L);
    std::vector<char> *luabuf = reinterpret_cast<std::vector<char> *>(ud);
    *sz = luabuf->size();
    return luabuf->data();
}
