#ifndef UIEDIT_H
#define UIEDIT_H
#include "IUiSupportModel.h"
#include "MainStatusBar.h"

class MainWindow;
class UiEdit :public IUiModel
{
public:
    UiEdit();
    virtual ~UiEdit();

    //模块初始化.
    virtual int Init(std::shared_ptr<interface_comm> &comm);

    //前端数据读写.
    virtual int SetValue(const std::string &name,const UiVariant &var){
        Q_UNUSED(name);
        Q_UNUSED(var);
        return -1;
    }
    virtual int GetValue(const std::string &name,UiVariant &var){
        Q_UNUSED(name);
        Q_UNUSED(var);
        return -1;
    }
    //绑定数据变化回调.
    virtual int RegisterValueCB(const std::string &name,ValueCB &CB){
        Q_UNUSED(name);
        Q_UNUSED(CB);
        return -1;
    }


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

    //初始化参数检测
    void InitParms(){
       m_parms.push_back("bool(const std::string &)");
    }

protected:
    //对外接口
    bool SwitchScene(const std::string &name);

private:
     MainWindow* m_mainW;
     std::shared_ptr<interface_comm> m_comm;

     //参数检查
     std::vector< std::string > m_parms;
};

_API_UI IUiModel *CreateUiM();

#endif // UIEDIT_H
