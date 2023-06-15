#ifndef ITEMPROEDITDLGBASE_H
#define ITEMPROEDITDLGBASE_H
#include <QDialog>
#include <QTabWidget>
#include <QJsonObject>

#define SPACERITEM 500

class item_base;
class ItemProEditDlgBase : public QDialog
{
    Q_OBJECT
public:
    //类型，按钮类-显示类
    enum Item_Type
    {
        ButtonType = 0,
        DisplayType
    };

    ItemProEditDlgBase(Item_Type type,item_base *item,QWidget * parent = nullptr);
    virtual ~ItemProEditDlgBase(){}

    //初始化UI显示
    void InitUI();

    //刷新数据
    void RefreshUI();

    //保存
    void SaveData();

protected:
    //自定义属性页面
    virtual void InitCustomPage() = 0;
    virtual void RefreshCustomPage() = 0;
    virtual void SaveCustomPage() = 0;

    //动态图像页面
    void InitDynaGraph();
    void RefreshDynaGraphPage();
    void SaveDynaGraph();

    //控制设置页面
    void InitCtlSettings();
    void RefreshCtlSettingsPage();
    void SaveCtlSettings();

    //显示设置页面
    void InitPlaySettings();
    void RefreshPlaySettingsPage();
    void SavePlaySettings();

    //样式表页面
    void InitStyleSheet();
    void RefreshStyleSheetPage();
    void SaveStyleSheet();

protected slots:
    //确认取消
    void comfirmSlot();
    void cancleSlot();

protected:
    QTabWidget * m_tabW = nullptr;
    //类型，按钮类-显示类
    Item_Type m_type = ButtonType;

    QWidget *m_customPage;
    QWidget *m_dynaGraph;
    QWidget *m_ctlSettings;
    QWidget *m_displaySettings;
    QWidget *m_styleSheet;

    QJsonObject m_itemObj;
    item_base * m_item;
};

#endif // ITEMPROEDITDLGBASE_H
