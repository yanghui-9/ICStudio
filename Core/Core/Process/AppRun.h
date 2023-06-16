#ifndef APPRUN_H
#define APPRUN_H
#include <memory>
#include <string>
#include "../../App/IAppRun.h"
#include "../Interface/IFuncModel.h"
#include "../Interface/IUiSupportModel.h"
#include "FuncManager.h"
#include "UISupportManager.h"
#include <QDebug>
#include <QDateTime>

class UiSupport;
class interface_comm;
class AppRun : public IAppR
{
public:
    struct CycRunModelType
    {
      FuncModel* model;
      int CycleIntervalTime = 0;
      QDateTime RecentSchedulingTime;
    };
    explicit AppRun(int n_threads = 10);
    virtual ~AppRun();

    //初始化.
    void InitRun();

    //通过ID获取模块指针.
    virtual ZObject * GetFuncModelFromID(int ID)
    {
        if(static_cast<uint>(ID) < m_models.size() && m_models.at(static_cast<uint>( ID )))
        {
           return m_models.at(static_cast<uint>( ID )).get();
        }
        return nullptr;
    }
    //事件分发器.
    bool Event(HEvent * event);

protected:
    //处理事件注册.
    void DealRegisteredEvents(HEvent *event);
    //建立模块对应索引关系.
    void BuildingModelOfIndex();
    //建立事件绑定.
    void BuildingEventBind();
    //功能注册id解密
    void DecryptionFuncIdFromFuncName(int &index,const std::string &name);
    //处理业务插件循环间隔调用
    void DealModelCycleRun();
    //业务插件循环间隔调用
    void ModelCycleRun();

private:
    std::shared_ptr<IUiModel> m_UiSupport;//前端插件指针
    std::vector< std::shared_ptr<ZObject> > m_funcModel;//后台业务插件列表
    std::shared_ptr<interface_comm> m_comm;//外设接口指针
    FuncManager m_funcManager;//后台插件管理模块
    UISupportManager m_uiSupportManager;//前端插件管理模块

    //功能调用转发
    std::vector<std::shared_ptr<ZObject>> m_models;//模块指针
    //事件驱动
    std::map<int ,std::vector<std::shared_ptr<ZObject>> > m_eventRegisterMap;//事件和扩展模块绑定关系

    //循环调用线程
    std::shared_ptr<std::thread> m_processThread;
    bool m_bThreadFlag = true;
    std::vector<CycRunModelType> m_cycleRunModels;//循环间隔调用模块指针列表
};

#endif // APPRUN_H
