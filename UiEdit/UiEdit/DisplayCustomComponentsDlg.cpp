#include "DisplayCustomComponentsDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QLabel>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "UWidgetScene.h"
#include "MainWindow.h"

DisplayCustomComponentsDlg::DisplayCustomComponentsDlg(MainWindow *mainW, QWidget *parent):QDialog(parent),m_mainW(mainW)
{
    setMinimumSize(400,200);
    setWindowTitle(QString::fromLocal8Bit("自定义组件"));
}

void DisplayCustomComponentsDlg::InitUI()
{
   //开始初始化界面
   QHBoxLayout * mainLayout = new QHBoxLayout(this);

   //组件列表
   m_imgListWidget = new QListWidget(this);
   //m_imgListWidget->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
   m_imgListWidget->setResizeMode(QListView::Adjust);
   m_imgListWidget->setMovement(QListView::Static);
   //m_imgListWidget->setSpacing(5);
   m_imgListWidget->setViewMode(QListView::IconMode);
   m_imgListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
   connect(m_imgListWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(itemClicked(QListWidgetItem*)));

   mainLayout->addWidget(m_imgListWidget);
}

void DisplayCustomComponentsDlg::UShow()
{
    //m_butLayout->setEnabled(false);
    resize(400,200);
    raise();
    show();
}


void DisplayCustomComponentsDlg::itemClicked(QListWidgetItem *item)
{
    if(m_mainW)
    {
        m_mainW->CustomItemselectDeal(item->text());
    }
}

void DisplayCustomComponentsDlg::LoadCombineItems(const QString &sList)
{
    if(m_sItemList == sList)
    {
        return;
    }
    m_sItemList = sList;
    m_imgListWidget->clear();
    QStringList itemList = sList.split('#');
    QListWidgetItem * pItem = nullptr;
    for (const auto &item : itemList)
    {
        if(!item.isEmpty())
        {
            QString pngPath = QString("./CustomComponents/%1.png").arg(item);
            if(!QFile::exists(pngPath))
            {
                continue;
            }
            pItem = new QListWidgetItem(QIcon(QPixmap(pngPath).scaled(QSize(20,20))),item);
            m_imgListWidget->addItem(pItem);
        }
    }
}
