#ifndef IAPP_H
#define IAPP_H
#include <string>
#include <qglobal.h>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>
#include "extend_func_def.h"

#  if defined(_APP_)
#    define Y_EXPORT Q_DECL_EXPORT
#  else
#    define Y_EXPORT Q_DECL_IMPORT
#  endif

#define zApp (IAPP::GetIntance())

//事件优先级
enum Event_Pri_Type
{
    Event_Hi = 0,
    Event_Nor,
    Event_Low
};
//注册功能调用类型
enum Func_Call_Type
{
    Func_Call_Sync = 0,//同步调用
    Func_Call_Asyn     //异步调用
};

class IUiModel;
class ZObject;
class HEvent;
class Y_EXPORT IAPP
{
    friend class IAppR;
public:
    //日志类型.
    enum Log_Type
    {
       Log_Communication = 0,
       Log_Error_Serious,
       Log_Error_Slight,
       Log_Operate
    };

    static IAPP * GetIntance(){
        return Instance;
    }
    virtual ~IAPP(){}

    //线程池任务入队.
    template<typename F, typename...Args>
    auto Submit(Event_Pri_Type type ,F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        // Create a function with bounded parameters ready to execute
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // Encapsulate it into a shared ptr in order to be able to copy construct / assign
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        // Wrap packaged task into void function
        std::function<void()> wrapper_func = [task_ptr]() {
            (*task_ptr)();
        };
        //Add
        AddTask(wrapper_func,type);
        // Return future from promise
        return task_ptr->get_future();
    }
    template<typename F, typename...Args>
    auto Submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        // Create a function with bounded parameters ready to execute
        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        // Encapsulate it into a shared ptr in order to be able to copy construct / assign
        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
        // Wrap packaged task into void function
        std::function<void()> wrapper_func = [task_ptr]() {
            (*task_ptr)();
        };
        //Add
        AddTask(wrapper_func,Event_Nor);
        // Return future from promise
        return task_ptr->get_future();
    }


    //线程池任务入队.
    virtual void AddTask(std::function<void()> &task,Event_Pri_Type type = Event_Nor) = 0;

    //事件处理
    //异步
    virtual void PostEvent(HEvent *event,Event_Pri_Type type = Event_Nor) = 0;
    //同步
    virtual void SendEvent(HEvent *event) = 0;

    //获取模块对象通过名称
    virtual int GetModelFromName(const std::string &name, std::shared_ptr<ZObject> &model) = 0;

    //注册功能调用
    virtual bool InvokeMethod(uint ID,Func_Call_Type callType,
                                   ZGenericReturnArgument ret,
                                   ZGenericArgument val0 = ZGenericArgument(),
                                   ZGenericArgument val1 = ZGenericArgument(),
                                   ZGenericArgument val2 = ZGenericArgument(),
                                   ZGenericArgument val3 = ZGenericArgument(),
                                   ZGenericArgument val4 = ZGenericArgument(),
                                   ZGenericArgument val5 = ZGenericArgument()) = 0;
    virtual bool InvokeMethod(uint ID,Func_Call_Type callType,
                                   ZGenericArgument val0 = ZGenericArgument(),
                                   ZGenericArgument val1 = ZGenericArgument(),
                                   ZGenericArgument val2 = ZGenericArgument(),
                                   ZGenericArgument val3 = ZGenericArgument(),
                                   ZGenericArgument val4 = ZGenericArgument(),
                                   ZGenericArgument val5 = ZGenericArgument()) = 0;
    bool InvokeMethod(uint ID,
                      ZGenericReturnArgument ret,
                      ZGenericArgument val0 = ZGenericArgument(),
                      ZGenericArgument val1 = ZGenericArgument(),
                      ZGenericArgument val2 = ZGenericArgument(),
                      ZGenericArgument val3 = ZGenericArgument(),
                      ZGenericArgument val4 = ZGenericArgument(),
                      ZGenericArgument val5 = ZGenericArgument())
    {
        return InvokeMethod(ID,Func_Call_Sync,ret,val0,val1,val2,val3,val4,val5);
    }
    bool InvokeMethod(uint ID,
                      ZGenericArgument val0 = ZGenericArgument(),
                      ZGenericArgument val1 = ZGenericArgument(),
                      ZGenericArgument val2 = ZGenericArgument(),
                      ZGenericArgument val3 = ZGenericArgument(),
                      ZGenericArgument val4 = ZGenericArgument(),
                      ZGenericArgument val5 = ZGenericArgument())
    {
        return InvokeMethod(ID,Func_Call_Sync,val0,val1,val2,val3,val4,val5);
    }

private:
    static IAPP * Instance;
};

#endif // IAPP_H
