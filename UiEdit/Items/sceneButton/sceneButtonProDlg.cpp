#include "sceneButtonProDlg.h"
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

sceneButtonProDlg::sceneButtonProDlg(item_base *item, QWidget *parent):ItemProEditDlgBase (ButtonType,item,parent)
{
    setWindowTitle(QString::fromLocal8Bit("画面按钮属性编辑"));
}

void sceneButtonProDlg::InitCustomPage()
{
    QVBoxLayout * mainLayout = new QVBoxLayout();

    //背景设置
    m_bgW = new BackgroundSettingsW(m_item,this);
    mainLayout->addWidget(m_bgW);

    //标签
    m_label = new LabelBase(this);
    mainLayout->addWidget(m_label);


    //功能
    QGroupBox * switchBox = new QGroupBox(QString::fromLocal8Bit("切换画面"),this);
    QHBoxLayout * swiLayout = new QHBoxLayout();
    QLabel * sceneLable = new QLabel(QString::fromLocal8Bit("画面："));
    m_sceneBox = new QComboBox(this);
    swiLayout->addWidget(sceneLable);
    swiLayout->addWidget(m_sceneBox);
    swiLayout->addSpacerItem(new QSpacerItem(SPACERITEM,1));
    switchBox->setLayout(swiLayout);
    mainLayout->addWidget(switchBox);

    m_customPage->setLayout(mainLayout);
}

void sceneButtonProDlg::RefreshCustomPage()
{
    //读取画面列表
    QDir dir("./project/scene");
    QFileInfoList fileList=dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::Hidden|QDir::NoDotAndDotDot,QDir::Name);
    for(int i = 0; i < fileList.size();i++)
    {
        if(fileList.at(i).suffix() == "scene")
        {
            m_sceneList.append(fileList.at(i).fileName().split('.').first());
        }
    }
    m_sceneBox->addItems(m_sceneList);

    QJsonObject obj;
    m_item->GetPro(obj);
    if(0 == obj["background_type"].toInt(0))
    {
        m_bgW->IsImgEnable(false);
        int color = obj["background_color"].toInt(0xdcdcdc);
        m_bgW->SetColor(QColor((color>>16)&0xff,(color>>8)&0xff,color&0xff));
    }
    else
    {
        m_bgW->IsImgEnable(true);
        m_bgW->SetImg(obj["background_img"].toString());
    }
    m_sceneBox->setCurrentText(obj["scene"].toString());
    m_label->textEdit->setText(obj["text"].toString());
}

void sceneButtonProDlg::SaveCustomPage()
{
    QJsonObject obj;
    m_item->GetPro(obj);
    if(m_bgW->IsImg())
    {
        obj["background_type"] = 1;
        obj["background_img"] = m_bgW->GetImg();
    }
    else
    {
        obj["background_type"] = 0;
        QColor bgColor = m_bgW->GetColor();
        obj["background_color"] = (bgColor.red()<<16)+(bgColor.green()<<8)+bgColor.blue();
    }
    obj["scene"] = m_sceneBox->currentText();
    obj["text"] = m_label->textEdit->toPlainText();
    m_item->SetPro(obj);
}
