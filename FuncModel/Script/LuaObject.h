#ifndef LUAOBJECT_H
#define LUAOBJECT_H
#include <vector>
#include "../../CommunicationProtocol/common/protocol_type.h"

struct ULua_State;
struct lua_State;
class interface_comm;
class Script;
class LuaObject
{
public:
    LuaObject(ULua_State *L,Script *script);

    //通过脚本文件初始化脚本对象
    //返回结果：成功or失败
    bool InitFromFile(const std::string &file);

    //运行脚本
    bool RunScript();

    //触发脚本回调绑定
    bool TriggerCallbackBind(interface_comm * comm);

    //初始化脚本二进制内容
    static bool InitLuaText(lua_State *L,const std::string &text, std::vector<char> &buff);

    //触发执行
    void TriggerCB(const Protocol::AddrInfoForRW &addr,int32_t code);

public:
    std::string m_name;//脚本名称
    std::string m_contend;//脚本内容
    std::vector<char> m_runBuff;//脚本二进制执行数据

    int m_luaType;//脚本类型
    Protocol::AddrInfoForRW m_triggerVar;//触发地址
    int m_cycTime;//循环脚本间隔时间
    ULua_State *m_L;
    Script * m_ScriptMdeol;
};

#endif // LUAOBJECT_H
