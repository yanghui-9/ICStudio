#include "GalleryDlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include <QLabel>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include "Gallery.h"

GalleryDlg::GalleryDlg(Gallery *gallery,QWidget *parent):QDialog(parent),m_gallery(gallery)
{
    setMinimumSize(800,600);
    setWindowTitle(QString::fromLocal8Bit("图库"));
}

void GalleryDlg::InitUI()
{
   //先读图片资源
   InitPic();

   //开始初始化界面
   QHBoxLayout * mainLayout = new QHBoxLayout(this);
   QVBoxLayout * leftLayout = new QVBoxLayout();

   //图片列表
   m_imgListWidget = new QListWidget(this);
   m_imgListWidget->setIconSize(QSize(ICON_WIDTH, ICON_HEIGHT));
   m_imgListWidget->setResizeMode(QListView::Adjust);
   m_imgListWidget->setMovement(QListView::Static);
   m_imgListWidget->setSpacing(10);
   m_imgListWidget->setViewMode(QListView::IconMode);
   m_imgListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
   connect(m_imgListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(itemDoubleClicked(QListWidgetItem*)));

   //图库类型选择
   QHBoxLayout * leftTopLayout = new QHBoxLayout();
   QLabel * typeLabel = new QLabel(QString::fromLocal8Bit("图库类型选择："),this);
   m_typeCombox = new QComboBox(this);
   leftTopLayout->addWidget(typeLabel);
   leftTopLayout->addWidget(m_typeCombox);
   connect(m_typeCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(typeChangeSlot(int)));
   for (QMap<QString,QList<QListWidgetItem*>>::Iterator it = m_picMap.begin(); it != m_picMap.end();++it)
   {
       m_typeCombox->addItem(it.key());
   }
   leftTopLayout->addSpacerItem(new QSpacerItem(1000,1));


   leftLayout->addLayout(leftTopLayout);
   leftLayout->addWidget(m_imgListWidget);

   QVBoxLayout * butLayout = new QVBoxLayout();
   m_confirm = new QPushButton(QString::fromLocal8Bit("确认"),this);
   connect(m_confirm,SIGNAL(clicked()),this,SLOT(selectComfirmSlot()));
   m_cancle = new QPushButton(QString::fromLocal8Bit("取消"),this);
   connect(m_cancle,SIGNAL(clicked()),this,SLOT(cancleSlot()));
   m_new = new QPushButton(QString::fromLocal8Bit("新增"),this);
   connect(m_new ,SIGNAL(clicked()),this,SLOT(addSlot()));
   m_del = new QPushButton(QString::fromLocal8Bit("删除"),this);
   connect(m_del,SIGNAL(clicked()),this,SLOT(delSlot()));
   butLayout->addWidget(m_confirm);
   butLayout->addWidget(m_cancle);
   butLayout->addWidget(m_new);
   butLayout->addWidget(m_del);
   butLayout->setSpacing(20);
   butLayout->addSpacerItem(new QSpacerItem(1,1000));

   mainLayout->addLayout(leftLayout);
   mainLayout->addLayout(butLayout);
}

void GalleryDlg::InitPic()
{
    //读取支持的协议列表.
    QDir Dir("./image");
    QFileInfoList folder_list = Dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString typeName;
    QList<QListWidgetItem *> itemList;
    for(int i = 0; i < folder_list.size(); i++)
    {
        typeName = folder_list.at(i).fileName();
        itemList.clear();
        QDir dir2(folder_list.at(i).absoluteFilePath());
        QFileInfoList file_list = dir2.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        std::shared_ptr<QImage> img = nullptr;
        for (int j = 0; j < file_list.size(); ++j)
        {
            img = std::make_shared<QImage>();
            img->load(file_list.at(j).filePath());
            QListWidgetItem * pItem = new QListWidgetItem(QIcon(QPixmap::fromImage(*img).scaled(QSize(ICON_WIDTH, ICON_HEIGHT))),
                                                          file_list.at(j).fileName());
            itemList.append(pItem);
            m_gallery->m_imgMap.insert(file_list.at(j).fileName().toStdString(),img);
        }
        m_picMap.insert(typeName,itemList);
    }
}

void GalleryDlg::typeChangeSlot(int index)
{
    Q_UNUSED(index);
    //清除
    while(m_imgListWidget->count()>0)
    {
        m_imgListWidget->takeItem(0);
    }
    //添加
    QMap<QString,QList<QListWidgetItem*>>::Iterator it = m_picMap.find(m_typeCombox->currentText());
    if(it != m_picMap.end())
    {
        foreach (auto item, it.value()) {
            m_imgListWidget->addItem(item);
        }
    }
}

void GalleryDlg::selectComfirmSlot()
{
    QList<QListWidgetItem*> selectItems = m_imgListWidget->selectedItems();
    if(0 < selectItems.size())
    {
        m_picPath = selectItems.at(0)->text();
    }
    accept();
}

void GalleryDlg::cancleSlot()
{
    reject();
}

void GalleryDlg::addSlot()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    QString::fromLocal8Bit("选择一张图片"),
                                                    "",
                                                    QString::fromLocal8Bit("Image Files (*.png *.jpg *.bmp)"));
    if(!fileName.isEmpty())
    {
        //QImage *img = new QImage(fileName);
        std::shared_ptr<QImage> img = std::make_shared<QImage>(fileName);
        QString imgName = fileName.split('/').last();
        //图片文件复制到imgae目录下
        QFile::copy(fileName,QString("./image/%1/%2").arg(m_typeCombox->currentText()).arg(imgName));
        //添加控件
        QListWidgetItem * pItem = new QListWidgetItem(QIcon(QPixmap::fromImage(*img).scaled(QSize(ICON_WIDTH, ICON_HEIGHT))),imgName);
        m_imgListWidget->addItem(pItem);
        //添加到容器
        QMap<QString,QList<QListWidgetItem*>>::Iterator it = m_picMap.find(m_typeCombox->currentText());
        if(it != m_picMap.end())
        {
            it.value().push_back(pItem);
        }
        m_gallery->m_imgMap.insert(imgName.toStdString(),img);
    }
}

void GalleryDlg::delSlot()
{
    QList<QListWidgetItem*> selectItems = m_imgListWidget->selectedItems();
    if(0 < selectItems.size())
    {
        foreach (QListWidgetItem* item, selectItems) {
            //文件删除
            QFile::remove(QString("./image/%1/%2").arg(m_typeCombox->currentText()).arg(item->text()));
            //容器删除
            QMap<QString,QList<QListWidgetItem*>>::Iterator it = m_picMap.find(m_typeCombox->currentText());
            if(it != m_picMap.end())
            {
                it.value().removeOne(item);
            }
            m_gallery->m_imgMap.remove(item->text().toStdString());
            //界面删除
            m_imgListWidget->removeItemWidget(item);
            delete item;
        }
    }
}

void GalleryDlg::itemDoubleClicked(QListWidgetItem *item)
{
    QDialog tmDlg(this);
    tmDlg.setWindowTitle(QString::fromLocal8Bit("图片名称修改"));
    QHBoxLayout * mainLayout = new QHBoxLayout(&tmDlg);
    QLabel *lable = new QLabel(QString::fromLocal8Bit("新名称："),&tmDlg);
    QLineEdit * edit = new QLineEdit(&tmDlg);
    connect(edit,&QLineEdit::returnPressed,[&,this](){
        tmDlg.accept();
        //先判断是否重名
        QMap<std::string,std::shared_ptr<QImage> >::Iterator it0 = m_gallery->m_imgMap.find(edit->text().toLocal8Bit().data());
        if(it0 != m_gallery->m_imgMap.end())
        {//重名退出
            QMessageBox::warning(this,QString::fromLocal8Bit("警告"),QString::fromLocal8Bit("图片名重复，无法修改！"));
            return;
        }
        //更新map值
        QMap<std::string,std::shared_ptr<QImage> >::Iterator it = m_gallery->m_imgMap.find(item->text().toLocal8Bit().data());
        if(it != m_gallery->m_imgMap.end())
        {
            m_gallery->m_imgMap.insert(edit->text().toLocal8Bit().data(),it.value());
            m_gallery->m_imgMap.remove(item->text().toLocal8Bit().data());
        }
        //修改源图片名称
        QFile::rename(QString("./image/%1/%2").arg(m_typeCombox->currentText()).arg(item->text()),
                      QString("./image/%1/%2").arg(m_typeCombox->currentText()).arg(edit->text()));
        //修改界面值
        item->setText(edit->text());
    });
    mainLayout->addWidget(lable);
    mainLayout->addWidget(edit);
    tmDlg.setLayout(mainLayout);
    tmDlg.exec();
}
