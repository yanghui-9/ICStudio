#ifndef ZOBJECT_H
#define ZOBJECT_H
#include <vector>
#include <string>

#define PROPERTY_MAX_SIZE 1024
#define Z_TRANSFORM(type,num) (*reinterpret_cast<type*>(_a[num]))
#define Z_TRANSFORM_R(type,_r) if (_a[0]) *reinterpret_cast<type*>(_a[0]) = std::move(_r)
//脚本接口指针类型
struct lua_State;
typedef int (*ScriptFunc)(lua_State *L);

class HEvent;
class ZObject
{
public:
    virtual ~ZObject(){}

    /*
     * 获取版本号
     * 参数1-v0-大版本号，参数2-v1-小版本号
    */
    virtual void GetVersion(uint32_t &v0,uint32_t &v1){
        //默认版本1.0
        v0 = 1;
        v1 = 0;
    }

    /////事件注册和执行
     /*
      * 模块注册事件绑定
      * 参数1-eventList-绑定的事件列表
      * return,执行返回结果
     */
    virtual int RegisterEvent(std::vector<int> &eventList){
         return -1;
     }
     /*
      * 事件触发执行接口
      * 参数1-event-事件指针（指针不需模块释放）
      * return,执行返回结果
     */
    virtual int EventCall(HEvent * event){
         return -1;
     }

    /////扩展功能注册和执行
    /*
     * 通过功能id执行模块提供的扩展功能
     * 参数1-funid-功能id，参数2-var-入参&出参
     * return,执行返回结果
    */
    virtual int ExecFuncFromID(int funID , void **_a){
        return -1;
    }
    /*
     * 检查参数
     * return,执行返回结果
    */
    virtual const char * CheckParms(int funID){
        return nullptr;
    }
    /*
     * 获取模块提供的扩展功能首id
     * return,执行返回结果
    */
    virtual int GetFuncID(){
        return -1;
    }
    /*
     * 获取模块名称
     * return,执行返回结果
    */
    virtual std::string GetFuncName(){
        return "";
    }
};


#endif
