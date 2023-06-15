#include "BackgroundSettingsW.h"
#include <QColorDialog>
#include <QVBoxLayout>
#include <QGridLayout>

#include "item_base.h"
#include "../../UiEdit/Gallery/interface_gallery.h"

BackgroundSettingsW::BackgroundSettingsW(item_base * item,QWidget *parent) :QGroupBox (parent),m_item(item)
{
    //背景设置
    setTitle(QString::fromLocal8Bit("背景设置"));

    QGridLayout * bgLayout = new QGridLayout();
    colorRadio = new QRadioButton(QString::fromLocal8Bit("纯色"),this);
    connect(colorRadio,SIGNAL(clicked()),this,SLOT(bgTypeChangeSlot()));
    colorBtn = new QPushButton(this);
    connect(colorBtn,SIGNAL(clicked()),this,SLOT(colorSelectSlot()));
    picRadio = new QRadioButton(QString::fromLocal8Bit("图片"),this);
    connect(picRadio,SIGNAL(clicked()),this,SLOT(bgTypeChangeSlot()));
    picBtn = new QPushButton(QString::fromLocal8Bit("图片选择"),this);
    connect(picBtn,SIGNAL(clicked()),this,SLOT(imgSelectSlot()));
    bgLayout->addWidget(colorRadio,0,0,1,1);
    bgLayout->addWidget(colorBtn,0,1,1,1);
    bgLayout->addWidget(picRadio,1,0,1,1);
    bgLayout->addWidget(picBtn,1,1,1,1);

    setLayout(bgLayout);
}

void BackgroundSettingsW::colorSelectSlot()
{
    QColorDialog dlg(this);
    if(QColorDialog::Accepted == dlg.exec())
    {
        QColor col = dlg.selectedColor();
        //按钮设置背景色
        m_bgColor = col;
        colorBtn->setStyleSheet(QString("background-color:#%1%2%3")
                                .arg(col.red(),2,16,QChar('0'))
                                .arg(col.green(),2,16,QChar('0'))
                                .arg(col.blue(),2,16,QChar('0')));
    }
}

void BackgroundSettingsW::imgSelectSlot()
{
    picBtn->setText(QString::fromLocal8Bit( m_item->m_gallery->ShowImgSelectDialog(this).data() ));
}

void BackgroundSettingsW::bgTypeChangeSlot()
{
    if(colorRadio->isChecked())
    {
        colorBtn->setEnabled(true);
        picBtn->setEnabled(false);
    }
    else
    {
        colorBtn->setEnabled(false);
        picBtn->setEnabled(true);
    }
}
