#include "ItemProEditDlgBase.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include "item_base.h"

ItemProEditDlgBase::ItemProEditDlgBase(Item_Type type, item_base *item, QWidget *parent)
    :QDialog (parent),m_type(type),m_item(item)
{
    setMinimumSize(600,400);
    //setMaximumSize(1024,600);
}

void ItemProEditDlgBase::InitUI()
{
    QVBoxLayout * mainLayout = new QVBoxLayout(this);

    //属性tab页面
    m_tabW = new QTabWidget(this);
    //控件自定义页面
    m_customPage = new QWidget(this);
    InitCustomPage();
    m_tabW->addTab(m_customPage,QString::fromLocal8Bit("一般设置"));
    //动态图形
    m_dynaGraph = new QWidget(this);
    InitDynaGraph();
    m_tabW->addTab(m_dynaGraph,QString::fromLocal8Bit("动态图形"));
    if(ButtonType == m_type)
    {
        //控制设置
        m_ctlSettings = new QWidget(this);
        InitCtlSettings();
        m_tabW->addTab(m_ctlSettings,QString::fromLocal8Bit("控制设置"));
    }
    //显示设置
    m_displaySettings = new QWidget(this);
    InitPlaySettings();
    m_tabW->addTab(m_displaySettings,QString::fromLocal8Bit("显示设置"));
    //样式表
    m_styleSheet = new QWidget(this);
    InitStyleSheet();
    m_tabW->addTab(m_styleSheet,QString::fromLocal8Bit("样式设置"));

    mainLayout->addWidget(m_tabW);

    //确认取消
    QHBoxLayout * btnLayout = new QHBoxLayout();
    QPushButton * comfirm = new QPushButton(QString::fromLocal8Bit("确认"),this);
    QPushButton * cancle = new QPushButton(QString::fromLocal8Bit("取消"),this);
    connect(comfirm,SIGNAL(clicked()),this,SLOT(comfirmSlot()));
    connect(cancle,SIGNAL(clicked()),this,SLOT(cancleSlot()));
    btnLayout->addSpacerItem(new QSpacerItem(SPACERITEM,1));
    btnLayout->addWidget(comfirm);
    btnLayout->addWidget(cancle);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);
}

void ItemProEditDlgBase::RefreshUI()
{
    RefreshCustomPage();
    RefreshDynaGraphPage();
    RefreshCtlSettingsPage();
    RefreshPlaySettingsPage();
    RefreshStyleSheetPage();
}

void ItemProEditDlgBase::SaveData()
{
    SaveCustomPage();
    SaveDynaGraph();
    SaveCtlSettings();
    SavePlaySettings();
    SaveStyleSheet();
}

void ItemProEditDlgBase::InitDynaGraph()
{
    //
}

void ItemProEditDlgBase::RefreshDynaGraphPage()
{
    //
}

void ItemProEditDlgBase::SaveDynaGraph()
{
    //
}

void ItemProEditDlgBase::InitCtlSettings()
{
    //
}

void ItemProEditDlgBase::RefreshCtlSettingsPage()
{
    //
}

void ItemProEditDlgBase::SaveCtlSettings()
{
    //
}

void ItemProEditDlgBase::InitPlaySettings()
{
    //
}

void ItemProEditDlgBase::RefreshPlaySettingsPage()
{
    //
}

void ItemProEditDlgBase::SavePlaySettings()
{
    //
}

void ItemProEditDlgBase::InitStyleSheet()
{
    //
}

void ItemProEditDlgBase::RefreshStyleSheetPage()
{
    //
}

void ItemProEditDlgBase::SaveStyleSheet()
{
    //
}

void ItemProEditDlgBase::comfirmSlot()
{
    SaveData();
    accept();
}

void ItemProEditDlgBase::cancleSlot()
{
    reject();
}
