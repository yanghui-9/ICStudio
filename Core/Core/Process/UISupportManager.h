#ifndef UISupportManager_H
#define UISupportManager_H
#include "../Interface/IUiSupportModel.h"
#include <QMap>
#include <QLibrary>


class UISupportManager
{
    typedef IUiModel*  (*P_CreateUiM)();
public:
    UISupportManager();
    ~UISupportManager();

    //初始化库加载
    IUiModel * InitLoad();

protected:
    void LoadProtocolFunc(const QString &pathFileName);

private:
    QList <QLibrary*> m_libs;
    QMap<QString,QString> m_supportLibNameMap;
    P_CreateUiM m_pFunc = nullptr;
};

#endif
