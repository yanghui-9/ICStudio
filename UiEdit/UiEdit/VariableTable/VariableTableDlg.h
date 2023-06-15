#ifndef VARIABLETABLEDLG_H
#define VARIABLETABLEDLG_H
#include <QDialog>
#include <QTableWidget>
#include <QPushButton>
#include <QMap>
#include <QComboBox>
#include <QString>
#include "protocol_type.h"

#define NEW_ADD_NANE "name"

class VariableTable;
class VariableTableDlg : public QDialog
{
    Q_OBJECT
public:
    enum Table_Type
    {
        varName = 0,
        linkName,
        reg,
        index,
        dataType,
        len,
        Table_Type_Max
    };
    const QString Table_Type_Name[Table_Type_Max] =
    {
        QString::fromLocal8Bit("变量名"),
        QString::fromLocal8Bit("连接名称"),
        QString::fromLocal8Bit("寄存器"),
        QString::fromLocal8Bit("索引"),
        QString::fromLocal8Bit("数据类型"),
        QString::fromLocal8Bit("长度")
    };
    const QStringList DataType_Names = {"bit","ubit8","bit8","ubit16","bit16","ubit32","bit32","float32","ubit64","bit64","double64","string"};

    explicit VariableTableDlg(VariableTable *variable,QWidget *parent = nullptr);

    //初始化UI显示
    void InitUI();

    //初始化变量表内容
    void InitVar();

    //获取选中确认
    QString GetSelect(){
        return m_selectVar;
    }

public slots:
    //图库类型切换
    void typeChangeSlot(int index);
    //确认选中
    void selectComfirmSlot();
    //取消
    void cancleSlot();
    //新增
    void addSlot();
    //删除
    void delSlot();
    //新增
    void addTypeSlot();
    //删除
    void delTypeSlot();
    //保存
    void saveSlot();
    //数据类型变化
    void dataTypeChangeSlot(int index);
    //双击改名
    void doubleClickedSlot(QTableWidgetItem *item);
    //数据变化
    void cellChangedSlot(int row, int column);

protected:
    //加载变量表配置文件
    void loadVarsFile(const QString &filePath);
    //保存变量表配置文件
    void saveVarsFile(const QString &name);
    //初始化行数据通过地址信息数据
    void initRowFromAddr(int row,const QString &name,const Protocol::AddrInfoForRW &addr);

private:
    QTableWidget * m_varWidget = nullptr;
    QPushButton * m_confirm = nullptr;
    QPushButton * m_cancle = nullptr;
    QPushButton * m_new = nullptr;
    QPushButton * m_del = nullptr;
    QPushButton * m_newType = nullptr;
    QPushButton * m_delType = nullptr;
    QPushButton * m_save = nullptr;
    QComboBox * m_typeCombox;
    VariableTable * m_Variables = nullptr;
    QString m_selectVar;
    QMap<QString,QStringList> m_typeOfNameMap;
    int m_Newcout = 0;
};

#endif // VARIABLETABLEDLG_H
