#ifndef _IOP_DEF_H_
#define _IOP_DEF_H_

#include <mutex>
#include <vector>
#include <list>
#include "io_config.h"

#define IOP_ERR_SOCKET -1
#define IOP_ERR_NO_MEM -2
#define IOP_ERR_PROTOCOL -3
#define IOP_ERR_TIMEOUT -4
#define IOP_ERR_CLOSED -5

//读事件
#define EV_TYPE_READ                1
//写事件
#define EV_TYPE_WRITE               2
//创建事件
#define EV_TYPE_CREATE              4
//销毁事件
#define EV_TYPE_DESTROY             8
//定时器事件
#define EV_TYPE_TIMER               16
//超时(客户端活跃超时)
#define EV_TYPE_TIMEOUT				32
//错误事件
#define EV_TYPE_ERROR               64
//连接创建事件
#define EV_TYPE_CONNECT             128
#define IOP_ERR_SYS  -1

//一组操作事件类型的宏
#define EV_IS_SET_R(e) ((e)&EV_TYPE_READ)
#define EV_IS_SET_W(e) ((e)&EV_TYPE_WRITE)
#define EV_IS_SET(e,x) ((e)&x)

#define EV_SET_R(e) ((e) | EV_TYPE_READ)
#define EV_CLR_R(e) ((e) & ~EV_TYPE_READ)
#define EV_SET_W(e) ((e) | EV_TYPE_WRITE)
#define EV_CLR_W(e) ((e) & ~EV_TYPE_WRITE)

#define EV_SET_RW(e) ((e)|EV_TYPE_READ|EV_TYPE_WRITE)
#define EV_TYPE_RW (EV_TYPE_READ|EV_TYPE_WRITE)

//系统支持的事件模型
//NULL代表由系统自动选择
#define IOP_MODEL_NULL                      0
//event ports模型暂未实现
#define IOP_MODEL_EVENT_PORTS       1
//kqueue模型暂未实现
#define IOP_MODEL_KQUEUE                  2
//linux epoll模型已经支持
#define IOP_MODEL_EPOLL                     3
//dev poll模型暂未实现
#define IOP_MODEL_DEV_POLL              4
//poll模型已经支持
#define IOP_MODEL_POLL                       5
//select 模型已经支持. windows & linux
#define IOP_MODEL_SELECT                   6
//windows iocp模型暂未实现
#define IOP_MODEL_IOCP                       7
//只支持定时器
#define IOP_MODEL_TIMER                     8

#define DEFAULT_DISPATCH_INTERVAL	500

/*max socket data buf:64M*/
#define IOP_MAX_BUF_SIZE   67108864
#define IOP_TYPE_FREE	0
#define IOP_TYPE_IO		1
#define IOP_TYPE_TIMER	2
#define IOP_TYPE_CONNECT	3



struct io_base_t;
struct io_t;
typedef int (*io_event_cb)(io_base_t *,int,unsigned int,void *);
typedef time_t io_time_t;

typedef void (*base_free)(io_base_t *);      //资源释放的接口
typedef int (*base_dispatch)(io_base_t *, int);  //模型调度接口
typedef int (*base_add)(io_base_t *, int, io_handle_t, unsigned int);//添加事件
typedef int (*base_del)(io_base_t *, int,io_handle_t);//删除事件
typedef int (*base_mod)(io_base_t *, int, io_handle_t, unsigned int);//修改事件

using ClientEventCB = std::function<void(io_t &,unsigned int)>;
using ServerEventCB = std::function<void(unsigned int)>;

struct io_op_t
{
    std::string name;    //模型名称
    base_free _base_free;//资源释放的接口
    base_dispatch _base_dispatch;//模型调度接口
    base_add _base_add;//添加事件
    base_del _base_del;//删除事件
    base_mod _base_mod;   //修改事件
};
struct io_t
{
    io_handle_t handle;		/*关联的句柄*/
    int timeout;			/*超时值*/
    io_event_cb evcb;		/*事件回调*/
    std::vector<char> sbuf;		/*发送缓存区*/
    std::vector<char> rbuf;		/*接收缓存区*/
    io_time_t last_dispatch_time;	/*上次调度的时间*/
    int port;
    std::string  sIp;

    bool operator == (const io_t &iop) const
    {
        if(handle == iop.handle)
        {
            return true;
        }
        return false;
    }
};

struct io_base_t
{
    io_handle_t handle;
    std::list<io_t> iops;		/*所有iop*/
    int maxio;			/*最大并发数*/
    uint32_t maxbuf;			/*单个发送或接收缓存的最大值*/

    int dispatch_interval;		/*select的超时时间*/

    io_op_t op_imp;           /*事件模型的内部实现*/
    void *model_data;         /*事件模型特定的数据*/

    ServerEventCB serverEventCB; /*服务端事件回调*/
    ClientEventCB clientEventCB; /*客户端事件回调*/

    io_time_t cur_time;		/*当前调度时间*/
    io_time_t last_time;		/*上次调度时间*/
    io_time_t last_keepalive_time; /*上次检查keepalive的时间*/
    std::mutex comm_lock; //锁
};

#define IOP_CB(base,iop,events)	{{if(0==(iop->evcb)(base, iop->id,events,iop->arg)){iop->last_dispatch_time = base->cur_time;}else{io_del(base,iop->id);}}}

#endif


