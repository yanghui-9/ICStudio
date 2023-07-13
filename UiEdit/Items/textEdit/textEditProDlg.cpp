#include "textEditProDlg.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QColorDialog>
#include <QDir>
#include <QTextEdit>

#include "../item_base.h"
#include "../BackgroundSettingsW.h"
#include "../LabelBase.h"

#include "../../UiEdit/Gallery/interface_gallery.h"
#include "../../UiEdit/VariableTable/interface_variable_table.h"

textEditProDlg::textEditProDlg(item_base *item, QWidget *parent):ItemProEditDlgBase (ButtonType,item,parent)
{
    setWindowTitle(QString::fromLocal8Bit("画面按钮属性编辑"));
}

void textEditProDlg::InitCustomPage()
{
    QVBoxLayout * mainLayout = new QVBoxLayout();

    //背景设置
    m_bgW = new BackgroundSettingsW(m_item,this);
    mainLayout->addWidget(m_bgW);

    //标签
    m_label = new LabelBase(this);
    mainLayout->addWidget(m_label);


    //功能
    QGroupBox * addrBox = new QGroupBox(QString::fromLocal8Bit("文本绑定:"),m_customPage);
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

    m_customPage->setLayout(mainLayout);
}

void textEditProDlg::RefreshCustomPage()
{
    QJsonObject obj;
    m_item->GetPro(obj);
    varBtn->setText(obj["var"].toString());
    m_label->textEdit->setText(obj["text"].toString());
}

void textEditProDlg::SaveCustomPage()
{
    QJsonObject obj;
    m_item->GetPro(obj);
    obj["text"] = m_label->textEdit->toPlainText();
    obj["var"] = varBtn->text();
    m_item->SetPro(obj);
}
