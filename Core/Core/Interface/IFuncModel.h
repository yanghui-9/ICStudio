#ifndef FUNCMODEL_H
#define FUNCMODEL_H
#include <memory>
#include "IZObject.h"

class interface_comm;
class FuncModel : public ZObject
{
public:
    virtual ~FuncModel(){}

    //模块初始化.
    virtual int Init(std::shared_ptr<interface_comm> &comm) = 0;

    //属性编辑
    virtual int ConfigEditShow(){
        return -1;
    }

    //Run循环
    //返回值：-1表示退出循环调用
    virtual int Run(){
        return -1;
    }

    //获取循环Run间隔时间单位ms
    //返回值：-1 表示无需循环Run,非-1表示循环调用间隔时间
    virtual int GetRunIntervalTime(){
        return -1;
    }
};


#if defined(API_FUNC)
#define _API_FUNC  extern "C" __declspec(dllexport)
#else
#define _API_FUNC  extern "C" __declspec(dllimport)
#endif


#endif // FUNCMODEL_H
