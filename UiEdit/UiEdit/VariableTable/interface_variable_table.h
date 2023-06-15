#ifndef INTERFACE_VARIABLE_TABLE_H
#define INTERFACE_VARIABLE_TABLE_H
#include <string>
#include "protocol_type.h"

class QWidget;
class interface_variable_table
{
public:
    virtual ~interface_variable_table(){}

    //变量表接口设计
    //显示模态显示图片选择对话框，返回选择的变量名称
    virtual std::string ShowVarSelectDialog(QWidget *parent) = 0;

    //通过变量名称获取地址数据
    virtual int GetAddrinfoFromVarName(const std::string &sPath,Protocol::AddrInfoForRW &addr) = 0;

};

#endif // INTERFACE_VARIABLE_TABLE_H
