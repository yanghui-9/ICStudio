#ifndef IUiModel_H
#define IUiModel_H
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <functional>
#include "IApp.h"
#include "IZObject.h"

class interface_comm;
class IUiModel : public ZObject
{
public:
    //前端数据结构.
    using UiVariant = std::variant<int64_t,double,std::string,std::map<std::string,std::string>,std::vector<std::string> >;
    //前端数据回调.
    using ValueCB = std::function<void (const std::string &,const UiVariant &)>;

    virtual ~IUiModel(){}

    //模块初始化.
    virtual int Init(std::shared_ptr<interface_comm> &comm) = 0;

    //设置日志.
    virtual int Log(IAPP::Log_Type type,const std::string &sErrCode,const std::string &overview) = 0;

    //前端数据读写.
    virtual int SetValue(const std::string &name,const UiVariant &var) = 0;
    virtual int GetValue(const std::string &name,UiVariant &var) = 0;
    //绑定数据变化回调.
    virtual int RegisterValueCB(const std::string &name,ValueCB &CB) = 0;

};


#if defined(API_UI)
#define _API_UI  extern "C" __declspec(dllexport)
#else
#define _API_UI  extern "C" __declspec(dllimport)
#endif


#endif // FUNCMODEL_H
