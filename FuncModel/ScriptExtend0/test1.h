#ifndef test1_H
#define test1_H
#include "IFuncModel.h"

class test1 :public FuncModel
{
public:
    test1(){}
    virtual ~test1(){}

    //模块初始化
    virtual int Init(std::shared_ptr<interface_comm> &comm);

protected:
    static int l_test1(lua_State *L);
};

_API_FUNC FuncModel *CreateFunc();

#endif // ZDK_AESMES_H
