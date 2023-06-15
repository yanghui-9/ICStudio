#ifndef VARIABLETABLE_H
#define VARIABLETABLE_H
#include <QMap>
#include "interface_variable_table.h"

class VariableTableDlg;
class VariableTable : public interface_variable_table
{
    friend class VariableTableDlg;
public:
    VariableTable();
    virtual ~VariableTable(){}

    //显示模态显示图片选择对话框，返回选择的变量名称
    virtual std::string ShowVarSelectDialog(QWidget *parent);

    //通过变量名称获取地址数据
    virtual int GetAddrinfoFromVarName(const std::string &sPath,Protocol::AddrInfoForRW &addr);

private:
    VariableTableDlg * m_dlg = nullptr;
    QMap<std::string,Protocol::AddrInfoForRW > m_varMap;
};

#endif // VARIABLETABLE_H
