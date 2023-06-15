#ifndef MACROINTERFACE_H
#define MACROINTERFACE_H
#include <vector>
#include "glog/logging.h"
#include "IZObject.h"
#define   LOG_HREAD "SCRIPT::"

class LuaFunc
{
public:
    //初始化
    static void Init();

    //外设接口
    static int l_SetValue(lua_State *L);
    static int l_GetValue(lua_State *L);
    static int l_SendData(lua_State *L);
    static int l_RecvData(lua_State *L);

    //字符编码转换
    static int l_Gbk2Utf(lua_State *L);
    static int l_Utf2Gbk(lua_State *L);

    //日志
    static int l_Log(lua_State *L);

    //脚本序列化
    static int myWriter(lua_State *L, const void *p, size_t sz, void *ud);
    static const char * myReader(lua_State *L, void *ud, size_t *sz);


    static std::vector<ScriptFunc> MFUNC_P;
    static std::vector<std::string> MFUNC_Name;
};

#endif // MACROINTERFACE_H
