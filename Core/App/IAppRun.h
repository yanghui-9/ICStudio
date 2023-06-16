#ifndef IAPPR_H
#define IAPPR_H
#include "IApp.h"
#include "SafeQueue.h"
#include "IZObject.h"

class Y_EXPORT IAppR : public IAPP
{
public:
    IAppR(const int n_threads);
    virtual ~IAppR();

    //任务入队
    virtual void AddTask(std::function<void()> &task,Event_Pri_Type type = Event_Nor);

    //事件处理
    //异步
    virtual void PostEvent(HEvent *event,Event_Pri_Type type = Event_Nor);
    //同步
    virtual void SendEvent(HEvent *event);
    //事件分发器
    virtual bool Event(HEvent * event) = 0;

    //注册功能调用
    virtual bool InvokeMethod(uint ID,Func_Call_Type callType,
                                   ZGenericReturnArgument ret,
                                   ZGenericArgument val0 = ZGenericArgument(),
                                   ZGenericArgument val1 = ZGenericArgument(),
                                   ZGenericArgument val2 = ZGenericArgument(),
                                   ZGenericArgument val3 = ZGenericArgument(),
                                   ZGenericArgument val4 = ZGenericArgument(),
                                   ZGenericArgument val5 = ZGenericArgument());
    virtual bool InvokeMethod(uint ID,Func_Call_Type callType,
                                   ZGenericArgument val0 = ZGenericArgument(),
                                   ZGenericArgument val1 = ZGenericArgument(),
                                   ZGenericArgument val2 = ZGenericArgument(),
                                   ZGenericArgument val3 = ZGenericArgument(),
                                   ZGenericArgument val4 = ZGenericArgument(),
                                   ZGenericArgument val5 = ZGenericArgument());
    //通过id获取扩展模块指针
    virtual ZObject * GetFuncModelFromID(int ID) = 0;

protected:
    //异步处理事件
    void PostEventDeal(HEvent *event);
    //检查日志目录
    void CheckCache();

private:
    class ThreadWorker {
    private:
        IAppR * m_pool;
    public:
        ThreadWorker(IAppR * pool)
            :m_pool(pool) {
        }

        void operator()() {
            std::function<void()> func;
            bool dequeued;
            while (!m_pool->m_shutdown) {
                {
                    std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);
                    if (m_pool->m_queue.empty() && m_pool->m_queueHi.empty() && m_pool->m_queueLo.empty()) {
                        m_pool->m_conditional_lock.wait(lock);
                    }
                    if(!m_pool->m_queueHi.empty()){
                        dequeued = m_pool->m_queueHi.dequeue(func);
                    }
                    else if(!m_pool->m_queue.empty()){
                        dequeued = m_pool->m_queue.dequeue(func);
                    }
                    else {
                        dequeued = m_pool->m_queueLo.dequeue(func);
                    }
                }
                if (dequeued) {
                    func();
                }
            }
        }
    };

public:
    //默认函数不生成
    IAppR(const IAppR &) = delete;
    IAppR(IAppR &&) = delete;
    IAppR & operator=(const IAppR &) = delete;
    IAppR & operator=(IAppR &&) = delete;
    //初始化线程池
    void Init();
    //等待线程完成当前任务并关闭池
    void shutdown();

private:
    bool m_shutdown;
    SafeQueue<std::function<void()>> m_queue;
    SafeQueue<std::function<void()>> m_queueHi;
    SafeQueue<std::function<void()>> m_queueLo;
    std::vector<std::thread> m_threads;
    std::mutex m_conditional_mutex;
    std::condition_variable m_conditional_lock;
};

#endif
