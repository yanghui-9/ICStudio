#ifndef FuncManager_H
#define FuncManager_H
#include "../Interface/IFuncModel.h"
#include <QMap>
#include <QLibrary>


class FuncManager
{
    typedef FuncModel *  (*P_CreateFunc)();
public:
    FuncManager();
    ~FuncManager();

    //初始化库加载
    void InitLoad(std::vector<std::shared_ptr<ZObject> > &modelVector);

protected:
    void LoadProtocolFunc(const QString &pathFileName);

private:
    QList <QLibrary*> m_libs;
    QMap<QString,QString> m_supportLibNameMap;
    P_CreateFunc m_pFunc = nullptr;
};

#endif // FuncManager_H
