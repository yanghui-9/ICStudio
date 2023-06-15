#ifndef SCRIPT_H
#define SCRIPT_H

#include <thread>
#include <map>
#include <vector>
#include <deque>
#include <mutex>

#include "IFuncModel.h"
#include "MacroInterface.h"
#include "LuaObject.h"
#include "interface_comm.h"

struct ULua_State
{
    lua_State *L = nullptr;
    std::mutex * lock = nullptr;
};

class Script : public FuncModel
{
public:
    Script();
    virtual ~Script();

    //初始化
    virtual int Init(std::shared_ptr<interface_comm> &comm);


    /////扩展功能注册和执行
    /*
     * 通过功能id执行模块提供的扩展功能
     * 参数1-funid-功能id，参数2-var-入参&出参
     * return,执行返回结果
    */
    virtual int ExecFuncFromID(int funID , void **_a);
    virtual const char * CheckParms(int funID);
    virtual int GetFuncID();
    virtual std::string GetFuncName();

    //插入触发脚本队列
    void InsertTrigger(LuaObject * obj);

protected:
    //lua
    int InitLua();
    int RunLua(const std::string &luaText);
    int RunLuaFromName(const std::string &name);

    //线程
    void InitThread();
    void StopThread();
    void RunScript();

    //执行开机脚本
    void RunRootScript();

    //扩展功能
    //1.注册脚本接口
    void RegisterScriptFunc(ScriptFunc func,std::string name);
    //2.外部通过名称调用脚本执行一次
    void RunScriptFromName(const std::string &name,bool isSync);

public:
    //外设
static std::shared_ptr<interface_comm> m_comm;


private:
    //脚本执行锁
    std::mutex m_lock;
    //参数检查
    std::vector< std::string > m_parms;
    //lua
    bool m_initLuaFlag = false;
    ULua_State m_L;
    std::vector<LuaObject*> m_cycLua;//循环脚本
    std::deque<LuaObject*> m_triggerLua;//已经的触发脚本
    std::mutex m_triggerLock;
    std::vector<std::string> m_conditionLua;//外部条件执行脚本
    std::mutex m_condLock;
    std::vector<LuaObject*> m_bootLua;//启动脚本列表
    //脚本名称对应脚本
    std::map<std::string,LuaObject*> m_nameOfLuaobjMap;
    //线程
    std::shared_ptr<std::thread> m_processThread;
    bool m_bThreadFlag = false;
    //外部注入脚本接口
    std::map<std::string,ScriptFunc> m_extendScriptFunc;
};

_API_FUNC FuncModel *CreateFunc();

#endif // SCRIPT_H
