#include "Itemsfactory.h"
#include <QLibrary>
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QIcon>
#include <QJsonDocument>
#include "UTabWidget.h"
#include "UWidgetScene.h"

itemsFactory * itemsFactory::instance = nullptr;

itemsFactory::itemsFactory(QObject *parent) : QObject(parent)
{
    init();
}

void itemsFactory::InitCombineItems()
{
    //列表.
    QDir Dir("./CustomComponents");
    QFileInfoList folder_list = Dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QString typeName;
    for(int i = 0; i < folder_list.size(); i++)
    {
        typeName = folder_list.at(i).fileName();
        QDir dir2(folder_list.at(i).absoluteFilePath());
        QFileInfoList file_list = dir2.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        for (int j = 0; j < file_list.size(); ++j)
        {
            if("item" == file_list.at(j).suffix())
            {
                LoadCombineItem(typeName+"/"+file_list.at(j).fileName().split('.').first(),file_list.at(j).filePath());
            }
        }
    }
}

void itemsFactory::LoadCombineItem(const QString &fileName ,const QString &filePath)
{
    QFile file(filePath);
    if(file.open(QIODevice::ReadOnly))
    {
        QJsonParseError jsonParserError;
        QJsonDocument outDoc = QJsonDocument::fromJson(file.readAll().constData(),&jsonParserError);
        if(!outDoc.isNull() && (jsonParserError.error == QJsonParseError::NoError))
        {
            m_combineItems.insert(fileName,outDoc.object());
        }
    }
}

itemsFactory::~itemsFactory()
{
    for(int i =0;i < m_listLib.size();i++)
    {
       if(m_listLib.at(i)->isLoaded())
       {
           m_listLib.at(i)->unload();
           m_listLib.at(i)->deleteLater();
       }
    }
    m_listLib.clear();
}

void itemsFactory::init()
{
    //加载库接口
    QDir dir("./items");
    QFileInfoList fileList=dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::Hidden|QDir::NoDotAndDotDot,QDir::Name);
    for(int i = 0; i < fileList.size();i++)
    {
        if(fileList.at(i).suffix() == "dll" || fileList.at(i).suffix() == "so")
        {
            QString fileName = fileList.at(i).absoluteFilePath();
            QLibrary *m_pLib = new QLibrary(fileName);
            if (m_pLib->load())
            {
                getItemName tmp0 =  (getItemName)m_pLib->resolve("getItemName");
                createItem tmp1 = (createItem)m_pLib->resolve("createItem");
                getItemAction tmp2 = (getItemAction)m_pLib->resolve("getItemAction");
                item_object_create tmp3 = (item_object_create)m_pLib->resolve("item_object_create");
                item_object_save tmp4 = (item_object_create)m_pLib->resolve("item_object_save");
                if(tmp0 && tmp1)
                {
                    std::string sName;
                    tmp0(sName);
                    QString itemsName = QString::fromStdString( sName );
                    QStringList nameList = itemsName.split(',');
                    for(int i = 0; i < nameList.size();i++)
                    {
                        QAction * action = nullptr;
                        if(tmp2)
                        {
                            action = tmp2( nameList.at(i).toStdString() );
                            action->setParent(this);
                            connect(action,SIGNAL(triggered()),this,SLOT(dealActionSlot()));
                        }
                        m_itemsAction.append(action);
                        m_listName.append(nameList.at(i));
                        m_list0.append(tmp1);
                        m_list1.append(tmp3);
                        m_list2.append(tmp4);
                    }
                    m_listLib.append(m_pLib);
                }
                else
                {
                   m_pLib->deleteLater();
                }
            }
            else
            {
                qDebug()<<m_pLib->errorString();
                m_pLib->deleteLater();
            }
        }
    }

    //加载自定义组件
    InitCombineItems();
}

QWidget *itemsFactory::createItemFromName(QString &name,QWidget *parent,bool bNoCustom)
{
    QWidget* item = nullptr;
    int index = m_listName.indexOf(name);
    if(index != -1)
    {
        if(index < m_list0.size())
        {
            createItem creat_item0 = m_list0.at(index);
            if(creat_item0)
            {
                item = creat_item0(parent);
            }
        }
    }
    else if(!bNoCustom)
    {//如果找不到就自定义组件找
        QMap<QString,QJsonObject>::Iterator it = m_combineItems.find(name);
        if(it != m_combineItems.end())
        {
            QJsonArray itemArray;
            itemArray.push_back(it.value());
            QList<QWidget*> listItems;
            m_view->curScene()->loadItemsFromJson(itemArray,listItems,parent);
            if(1 == listItems.size())
            {
               item = listItems.first();
            }
            name = COMBINE_ITEM;
        }
    }
    return item;
}

void itemsFactory::createItemObjFromName(const QString &className, QWidget *item, QJsonObject *object)
{
    int index = m_listName.indexOf(className);
    if(index != -1 && index < m_list1.size())
    {
        item_object_create creat_item0 = m_list1.at(index);
        if(creat_item0)
        {
            creat_item0(className.toStdString(),item, object);
        }
    }
}

void itemsFactory::saveItemObjFromName(const QString &className, QWidget *item, QJsonObject *object)
{
    int index = m_listName.indexOf(className);
    if(index != -1 && index < m_list2.size())
    {
        item_object_save save_item0 = m_list2.at(index);
        if(save_item0)
        {
            save_item0(className.toStdString(),item, object);
        }
    }
}

int itemsFactory::getItemProList(QList<QString> &nameList, QList<QAction *> &acList)
{
    nameList.append(m_listName);
    acList.append(m_itemsAction);
    return nameList.size();
}

itemsFactory *itemsFactory::GetInstance()
{
    //static itemsFactory instance;
    //return &instance;
    if(!instance)
    {
        instance = new itemsFactory();
    }
    return instance;
}

void itemsFactory::ReleaseInstace()
{
    if(instance)
    {
        delete instance;
        instance = nullptr;
    }
}

void itemsFactory::dealActionSlot()
{
    QAction * ac = qobject_cast<QAction *>(sender());
    if(ac)
    {
        if(m_view)
        {
            UWidgetScene * curSence = m_view->curScene();
            if(curSence)
            {
                curSence ->setCreateMode(ac->text());
            }
        }
    }
}

