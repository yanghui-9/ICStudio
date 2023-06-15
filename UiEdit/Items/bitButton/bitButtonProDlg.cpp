#include "bitButtonProDlg.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>

#include "../item_base.h"
#include "../../UiEdit/Gallery/interface_gallery.h"
#include "../../UiEdit/VariableTable/interface_variable_table.h"

bitButtonProDlg::bitButtonProDlg(item_base *item, QWidget *parent):ItemProEditDlgBase (ButtonType,item,parent)
{
    setWindowTitle(QString::fromLocal8Bit("位按钮属性编辑"));
}

void bitButtonProDlg::InitCustomPage()
{
    QVBoxLayout * mainLayout = new QVBoxLayout();

    QGroupBox * addrBox = new QGroupBox(QString::fromLocal8Bit("状态监控"),m_customPage);
    QHBoxLayout * addLayout = new QHBoxLayout(addrBox);
    QLabel * varLabel = new QLabel(QString::fromLocal8Bit("变量选择:"),m_customPage);
    varBtn = new QPushButton(m_customPage);
    connect(varBtn,&QPushButton::clicked,[=](){
        std::string sRet = m_item->m_variable->ShowVarSelectDialog(nullptr);
        if(!sRet.empty())
        {
            varBtn->setText(QString::fromLocal8Bit(sRet.data()));
        }
    });
    addLayout->addWidget(varLabel);
    addLayout->addWidget(varBtn);
    addLayout->addSpacerItem(new QSpacerItem(SPACERITEM,1));
    addrBox->setLayout(addLayout);
    mainLayout->addWidget(addrBox);

    QGroupBox * onBox = new QGroupBox("ON",m_customPage);
    QHBoxLayout * onLayout = new QHBoxLayout(onBox);
    QLabel * onPicLabel = new QLabel(QString::fromLocal8Bit("图片选择:"),m_customPage);
    onPicBtn = new QPushButton(m_customPage);
    connect(onPicBtn,&QPushButton::clicked,[=](){
        std::string sRet = m_item->m_gallery->ShowImgSelectDialog(nullptr);
        if(!sRet.empty())
        {
            onPicBtn->setText(QString::fromLocal8Bit(sRet.data()));
        }
    });
    onLayout->addWidget(onPicLabel);
    onLayout->addWidget(onPicBtn);
    onLayout->addSpacerItem(new QSpacerItem(SPACERITEM,1));
    onBox->setLayout(onLayout);
    mainLayout->addWidget(onBox);

    QGroupBox * offBox = new QGroupBox("OFF",m_customPage);
    QHBoxLayout * offLayout = new QHBoxLayout(offBox);
    QLabel * offPicLabel = new QLabel(QString::fromLocal8Bit("图片选择:"),m_customPage);
    offPicBtn = new QPushButton(m_customPage);
    connect(offPicBtn,&QPushButton::clicked,[=](){
        std::string sRet = m_item->m_gallery->ShowImgSelectDialog(nullptr);
        if(!sRet.empty())
        {
            offPicBtn->setText(QString::fromLocal8Bit(sRet.data()));
        }
    });
    offLayout->addWidget(offPicLabel);
    offLayout->addWidget(offPicBtn);
    offLayout->addSpacerItem(new QSpacerItem(SPACERITEM,1));
    offBox->setLayout(offLayout);
    mainLayout->addWidget(offBox);

    mainLayout->addSpacerItem(new QSpacerItem(1,SPACERITEM));
    m_customPage->setLayout(mainLayout);
}

void bitButtonProDlg::RefreshCustomPage()
{
    QJsonObject obj;
    m_item->GetPro(obj);
    onPicBtn->setText(obj["on_img"].toString());
    offPicBtn->setText(obj["off_img"].toString());
    varBtn->setText(obj["var"].toString());
}

void bitButtonProDlg::SaveCustomPage()
{
    QJsonObject obj;
    m_item->GetPro(obj);
    obj["on_img"] = onPicBtn->text();
    obj["off_img"] = offPicBtn->text();
    obj["var"] = varBtn->text();
    m_item->SetPro(obj);
}
