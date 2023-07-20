#include "UWidgetScene.h"
#include <QPainter>
#include <QFile>
#include <qmath.h>
#include <QDebug>
#include <QJsonParseError>
#include <QFileDialog>
#include "UTabWidget.h"
#include "MainStatusBar.h"
#include "Itemsfactory.h"
#include "MainWindow.h"
#include "interface_comm.h"
#include "Undo/UDelUndo.h"
#include "Undo/UAddUndo.h"
#include "UScenePropertyEditDlg.h"
#include "Gallery/interface_gallery.h"

#include "IApp.h"
#include "extend_func_head.h"

UWidgetScene::UWidgetScene(int type, QString name, UTabWidget *view):
    QWidget(nullptr) ,m_view(view),m_name(name)
{
    (void)type;
    //undo/redo
    m_sceneUndoStack = new QUndoStack(this);
    m_sceneUndoStack->setUndoLimit(UNDO_LIMIT);

    //当前模式
    m_currentPolicy = nullptr;

    //设置背景色
#if 0
    QPalette palette = this->palette();
    palette.setColor(QPalette::Window,QColor(220,220,220));
    setAutoFillBackground(true);
    this->setPalette(palette);
#endif

    //鼠标抓取move
    setMouseTracking(true);

    //模式获取
    if(m_view)
    {//设置控件的模式
        m_modeFlag = m_view->m_mainW->GetModeFalg();
    }

    //信号槽
    //画面循环脚本
    connect(&m_cycScript,SIGNAL(timeout()),this,SLOT(sceneCycScriptSlot()));
}

UWidgetScene::~UWidgetScene()
{
    //加上下面代码，退出时会报错，原因还未查到
    //foreach (auto item, m_itemList) {
    //   item->setParent(nullptr);
    //   item->deleteLater();
    //}
}


void UWidgetScene::loadItemsFromJson(QJsonArray &itemArray,QList<QWidget *> &itemList, QWidget *parent)
{
    QString className;
    foreach (auto itemObj, itemArray)
    {//item
        QJsonObject itemobj =  itemObj.toObject();
        className = itemobj.value("z_class_name").toString();
        QWidget *item = itemsFactory::GetInstance()->createItemFromName(className,parent);
        if(item)
        {
            itemsFactory::GetInstance()->createItemObjFromName(className,item,&itemobj);
            item->setProperty("z_class_name",className);
            itemList.append(item);
            if(COMBINE_ITEM == className)
            {
                QJsonArray itemArrayCombine = itemobj.value("z_items").toArray();
                QList<QWidget *> itemListCombine;
                loadItemsFromJson(itemArrayCombine,itemListCombine,item);
                foreach (auto itemChild, itemListCombine)
                {
                   initItem(itemChild,"",-1,-1,-1,-1);
                   //组合时的长宽用来后续尺寸计算
                   itemChild->setProperty(COMBINE_ITEM_X,itemChild->pos().x());
                   itemChild->setProperty(COMBINE_ITEM_Y,itemChild->pos().y());
                   itemChild->setProperty(COMBINE_ITEM_W,itemChild->width());
                   itemChild->setProperty(COMBINE_ITEM_H,itemChild->height());
                }
                //设置组合时的长宽用来后续尺寸计算
                item->setProperty(COMBINE_ITEM_PARENT_W,item->width());
                item->setProperty(COMBINE_ITEM_PARENT_H,item->height());
            }
        }
    }
}

void UWidgetScene::ShowWindow(QWidget *window, QRect rect)
{
    window->setParent(this);
    window->setGeometry(rect.x(),rect.y(),rect.width(),rect.height());
    window->setVisible(true);
}

void UWidgetScene::saveItemsFromJson(QJsonArray &itemArray,QList<QWidget *> &itemList)
{
    QString className;
    foreach (auto item, itemList)
    {//item
        QJsonObject itemValue;
        className = item->property("z_class_name").toString();
        itemsFactory::GetInstance()->saveItemObjFromName(className,item,&itemValue);
        //判断组合控件处理
        if(COMBINE_ITEM == className)
        {
           QList<QWidget*> itemlistCombine;
           QObjectList childList = item->children();
           //转换
           foreach (auto itemchild, childList) {
              itemlistCombine.push_back(qobject_cast<QWidget*>(itemchild));
           }
           QJsonArray itemArrayCombine;
           saveItemsFromJson(itemArrayCombine,itemlistCombine);
           itemValue["z_items"] = itemArrayCombine;
        }
        itemArray.append(itemValue);
    }
}

void UWidgetScene::dealItemPaste(QList<QWidget *> items,QList<QWidget *> itemsNewList, QWidget * parent)
{
    foreach (auto item, items)
    {
        //新增控件
        QString sClassName = item->property("z_class_name").toString();
        QWidget * newitem = itemsFactory::GetInstance()->createItemFromName(sClassName,parent);
        itemsFactory::GetInstance()->createItemObjFromName(sClassName,newitem);
        newitem->setProperty("z_class_name",sClassName);
        addItem(newitem, QPoint(item->pos().x()+10,item->pos().y()+10),QSize(item->width(),item->height()));
        //处理组合控件
        if( COMBINE_ITEM == sClassName)
        {
            QObjectList childList = item->children();
            //转换
            QList<QWidget *> itemsNewListOfcombine;
            QList<QWidget*> itemlistCombine;
            foreach (auto itemchild, childList)
            {
               itemlistCombine.push_back(qobject_cast<QWidget*>(itemchild));
            }
            dealItemPaste(itemlistCombine,itemsNewListOfcombine);

            //组合控件尺寸属性
            foreach (auto itemChild, itemlistCombine) {
                itemChild->setProperty(COMBINE_ITEM_X,item->pos().x());
                itemChild->setProperty(COMBINE_ITEM_Y,item->pos().y());
                itemChild->setProperty(COMBINE_ITEM_W,item->width());
                itemChild->setProperty(COMBINE_ITEM_H,item->height());
            }

            //设置组合时的长宽用来后续尺寸计算
            newitem->setProperty(COMBINE_ITEM_PARENT_W,newitem->width());
            newitem->setProperty(COMBINE_ITEM_PARENT_H,newitem->height());
        }
        //取消选择
        setSelected(item,false);
        //添加
        itemsNewList.append(newitem);
    }
}

void UWidgetScene::initItem(QWidget *newitem,const QString &sClassName,int x, int y, int w, int h)
{
    //属性
    if(!sClassName.isEmpty())
    {
        newitem->setProperty("z_class_name",sClassName);
    }
    if(-1 != x)
    {
        newitem->setGeometry(x,y,w,h);
    }
    newitem->setVisible(true);

    //初始化
    interface_item *iItem = dynamic_cast<interface_item*>(newitem);
    if(iItem)
    {//设置控件的模式
        if(m_modeFlag)
        {
            iItem->update_result(0x60);
        }
        else
        {
            iItem->update_result(0x20);
        }
        iItem->initItem(m_view->m_mainW->m_comm,
                        m_view->m_mainW->m_gallery,
                        m_view->m_mainW->m_varTable);
    }
}

void UWidgetScene::loadSceneItem(const QString &sdata)
{
    m_sceneObject = sdata;
    QJsonParseError jsonParserError;
    QJsonDocument outDoc = QJsonDocument::fromJson(m_sceneObject.toUtf8().data(),&jsonParserError);
    if(!outDoc.isNull() && (jsonParserError.error == QJsonParseError::NoError))
    {
        m_sceneObj = outDoc.object();
        //画面属性
        m_name = m_sceneObj["z_name"].toString();
        if(m_sceneObj.value("z_items").isArray())
        {//item array
            QJsonArray itemArray =  m_sceneObj.value("z_items").toArray();
            QList<QWidget*> itemList;
            loadItemsFromJson(itemArray,itemList,this);
            foreach (auto item, itemList) {
               addItem(item);
            }
        }
        //根据配置初始化画面显示
        initScenePropertyFromObj();
    }
}

void UWidgetScene::saveSceneItem(QString &sdata)
{
    //画面属性
    m_sceneObj["z_name"] = m_name;
    m_sceneObj["z_type"] = m_sceneObj["z_type"].toInt(UTabWidget::Scene);

    //处理画面控件属性保存
    QJsonArray itemsArray;
    saveItemsFromJson(itemsArray,m_itemList);
    m_sceneObj["z_items"] = itemsArray;

    //json转字节
    QJsonDocument document;
    document.setObject(m_sceneObj);
    sdata = document.toJson();
    m_sceneObject = sdata;
}

void UWidgetScene::DealAllItem(int flag)
{
    QList<QWidget*>list = items();
    for(int i = 0; i < list.size();i++)
    {
        interface_item* item = dynamic_cast<interface_item*>(list.at(i));
        if(item)
        {
           if(item->update_result(flag))
           {
               item->update();
           }
        }
    }
}

void UWidgetScene::initScenePropertyFromObj()
{
    //背景
    if(0 == m_sceneObj["background_type"].toInt(0))
    {
        int color = m_sceneObj["background_color"].toInt(0xdcdcdc);
        //按钮设置背景色
        QPalette palette = this->palette();
        palette.setColor(QPalette::Window,QColor((color>>16)&0xff,(color>>8)&0xff,color&0xff));
        setAutoFillBackground(true);
        this->setPalette(palette);
    }
    else
    {
        QPalette palette = this->palette();
        std::shared_ptr<QImage> img;
        m_view->m_mainW->m_gallery->GetImgFromPath(
                    m_sceneObj["background_img"].toString().toStdString(),img);
        if(img)
        {
            palette.setBrush(QPalette::Window,QBrush((*img).scaled(size())));
            this->setPalette(palette);
        }
    }

    //窗口
    if(1 == m_sceneObj["window"].toInt(0))
    {
        move(m_sceneObj["x"].toInt(0),m_sceneObj["y"].toInt(0));
        resize(m_sceneObj["w"].toInt(800),m_sceneObj["h"].toInt(600));
        m_sceneObj["z_type"] = UTabWidget::Window;
    }
}

void UWidgetScene::GetItemPro(QWidget *item, QJsonObject &obj)
{
    interface_item* iItem = dynamic_cast<interface_item*>(item);
    if(iItem)
    {
        iItem->update_result(ITEM_SET_FLAG(0,Item_Update_Flag_GetPro),&obj);
    }
}

void UWidgetScene::sceneCycScriptSlot()
{
    //执行脚本
    IAPP::GetIntance()->InvokeMethod(Extend_Script_RunFromName,
                                     Z_ARG(const std::string &,m_sceneObj["script_cycle_name"].toString().toLocal8Bit().data()),
                                     Z_ARG(bool,false));
}


void UWidgetScene::dealScenceClose()
{
    //画面关闭
    DealAllItem(ITEM_SET_FLAG(0,Item_Update_Flag_SceneClose));
    if(!m_modeFlag)
    {//运行模式
        //处理画面关闭脚本
        IAPP::GetIntance()->InvokeMethod(Extend_Script_RunFromName,
                                         Z_ARG(const std::string &, m_sceneObj["script_release_name"].toString().toLocal8Bit().data()),
                Z_ARG(bool,true));

        //循环脚本处理
        if(m_cycScript.isActive())
            m_cycScript.stop();
    }
}

void UWidgetScene::dealScenceOpen()
{
    //画面打开
    DealAllItem(ITEM_SET_FLAG(0,Item_Update_Flag_SceneOpen));
    if(!m_modeFlag)
    {//运行模式
        //处理变量注册
        AssociateAddressAndItem();
        //处理画面打开脚本
        IAPP::GetIntance()->InvokeMethod(Extend_Script_RunFromName,
                                         Z_ARG(const std::string &,m_sceneObj["script_press_name"].toString().toLocal8Bit().data()),
                                         Z_ARG(bool,true));
        //启动循环脚本
        if( m_sceneObj["script_cycle"].toInt() )
        {
            m_cycScript.start(m_sceneObj["script_cycle_interval"].toInt(1000));
        }
    }
    else
    {
        //窗口
        if(1 == m_sceneObj["window"].toInt(0))
        {
            move(m_sceneObj["x"].toInt(0),m_sceneObj["y"].toInt(0));
            resize(m_sceneObj["w"].toInt(800),m_sceneObj["h"].toInt(600));
        }
    }
}

void UWidgetScene::reflashAllItem()
{
    //主动检查当前画面是否需要刷新
    DealAllItem(ITEM_SET_FLAG(0,Item_Update_Flag_Refresh));
}

void UWidgetScene::removeItemObject(QList<QWidget *> items, bool isUndo)
{
    //item->deleteLater();
    foreach (auto item, items) {
        item->setParent(nullptr);
        item->setVisible(false);
        m_itemList.removeAll(item);
    }
    if(!isUndo)
    {
        m_sceneUndoStack->push(new UDelUndo(this,items));
    }
}

QRect UWidgetScene::getSceneRect()
{
    return QRect(m_sceneObj["x"].toInt(0),
            m_sceneObj["y"].toInt(0),
            m_sceneObj["w"].toInt(800),
            m_sceneObj["h"].toInt(600));
}

void UWidgetScene::ItemUndo()
{
    m_sceneUndoStack->undo();
}

void UWidgetScene::ItemRedo()
{
    m_sceneUndoStack->redo();
}

void UWidgetScene::ItemCopy()
{
    m_view->m_copyList.clear();
    QList<QWidget *> items = getOptItems();
    m_view->m_copyList.append(items);
}

void UWidgetScene::ItemPaste()
{
    QList<QWidget *> items = m_view->m_copyList;
    QList<QWidget *> itemsNewList;
    foreach (auto item, items)
    {
        //克隆控件
        itemsNewList.append(cloneOfItem(item,this));
        //取消选择
        if(this == item->parent())
        {
            setSelected(item,false);
        }
    }

    //新增的控件设置选择
    foreach (auto item, itemsNewList) {
        //复制的控件加上偏移
        item->setGeometry(item->pos().x()+10,
                          item->pos().y()+10,
                          item->width(),
                          item->height());
        m_itemList.push_back(item);
        setSelected(item,true);
    }

    //undo
    m_sceneUndoStack->push(new UAddUndo(this,itemsNewList));
}

void UWidgetScene::ItemDelete()
{
    QList<QWidget *> items = getOptItems();
    removeItemObject(items);
}

void UWidgetScene::ItemMvoe(UWidgetScene::Move_Type type)
{
    if(!m_modeFlag || !m_currentPolicy)
    {
        return;
    }
    switch (type) {
    case move_up:
    {
        QKeyEvent event(QEvent::KeyPress,Qt::Key_Up,Qt::NoModifier);
        m_currentPolicy->keyPressEvent(&event);
    }
        break;
    case move_down:
    {
        QKeyEvent event(QEvent::KeyPress,Qt::Key_Down,Qt::NoModifier);
        m_currentPolicy->keyPressEvent(&event);
    }
        break;
    case move_left:
    {
        QKeyEvent event(QEvent::KeyPress,Qt::Key_Left,Qt::NoModifier);
        m_currentPolicy->keyPressEvent(&event);
    }
        break;
    case move_right:
    {
        QKeyEvent event(QEvent::KeyPress,Qt::Key_Right,Qt::NoModifier);
        m_currentPolicy->keyPressEvent(&event);
    }
        break;
    }
}

void UWidgetScene::ItemLower()
{
     QList<QWidget *> itemsList = getOptItems();
     if(1 == itemsList.size())
     {
         int index = m_itemList.indexOf(itemsList.at(0));
         if(-1 != index)
         {
            m_itemList.at(index)->lower();
            m_itemList.push_front(m_itemList.at(index));
            m_itemList.removeAt(index);
         }
     }
}

void UWidgetScene::ItemRaise()
{
    QList<QWidget *> itemsList = getOptItems();
    if(1 == itemsList.size())
    {
        int index = m_itemList.indexOf(itemsList.at(0));
        if(-1 != index)
        {
           m_itemList.at(index)->raise();
           m_itemList.append(m_itemList.at(index));
           m_itemList.removeAt(index);
        }
    }
}

void UWidgetScene::ItemZvalueMinus()
{
    QList<QWidget *> itemsList = getOptItems();
    if(1 == itemsList.size())
    {
        int index = m_itemList.indexOf(itemsList.at(0));
        if(index > 0)
        {
            m_itemList.at(index)->stackUnder(m_itemList.at(index-1));
            m_itemList.swapItemsAt(index,index-1);
        }
    }
}

void UWidgetScene::ItemZvaluePlus()
{
    QList<QWidget *> itemsList = getOptItems();
    if(1 == itemsList.size())
    {
        int index = m_itemList.indexOf(itemsList.at(0));
        if(-1 != index && index != (m_itemList.size()-1) )
        {
            m_itemList.at(index+1)->stackUnder(m_itemList.at(index));
            m_itemList.swapItemsAt(index,index+1);
        }
    }
}

void UWidgetScene::ItemLock()
{
   QList<QWidget *> itemsList = selectedItems();
   foreach (auto item, itemsList) {
       item->setProperty("isLocked",true);
   }
}

void UWidgetScene::ItemUnLock()
{
    QList<QWidget *> itemsList = selectedItems();
    foreach (auto item, itemsList) {
        item->setProperty("isLocked",false);
    }
}

void UWidgetScene::ItemCombine()
{
    QList<QWidget *> itemsList = getOptItems();
    QWidget * itemCombine = nullptr;
    if(itemsList.size() > 0)
    {
        //创建combine控件
        QString className = COMBINE_ITEM;
        itemCombine = itemsFactory::GetInstance()->createItemFromName(className,this);
        itemsFactory::GetInstance()->createItemObjFromName(className,itemCombine);
        itemCombine->setProperty("z_class_name",className);
        //获取坐标和宽高
        int minX = 0xffff;
        int minY = 0xffff;
        int maxX = 0;
        int maxY = 0;
        foreach (auto item, itemsList)
        {
            if(item->x() < minX)
            {
               minX = item->x();
            }
            if(item->y() < minY)
            {
               minY = item->y();
            }
            if(maxX < (item->pos().x()+item->size().width()))
            {
                maxX = item->pos().x()+item->size().width();
            }
            if(maxY < (item->pos().y()+item->size().height()))
            {
                maxY = item->pos().y()+item->size().height();
            }
            setSelected(item,false);
            item->setParent(itemCombine);
            item->setVisible(true);
            m_itemList.removeOne(item);
        }
        addItem(itemCombine,QPoint(minX-5,minY-5),QSize(maxX-minX+10,maxY-minY+10));
        //坐标系转换
        foreach (auto item, itemsList)
        {
            item->move(item->x()-itemCombine->x(),item->y()-itemCombine->y());
            item->setProperty(COMBINE_ITEM_X,item->pos().x());
            item->setProperty(COMBINE_ITEM_Y,item->pos().y());
            item->setProperty(COMBINE_ITEM_W,item->size().width());
            item->setProperty(COMBINE_ITEM_H,item->size().height());
        }
        //记录组合时组合控件的长宽数据，后续尺寸变化时子控件计算比例使用
        itemCombine->setProperty(COMBINE_ITEM_PARENT_W,itemCombine->width());
        itemCombine->setProperty(COMBINE_ITEM_PARENT_H,itemCombine->height());
    }
}

void UWidgetScene::ItemUnCombine()
{
    QList<QWidget *> itemsList = getOptItems();
    foreach (auto item, itemsList)
    {
        if(COMBINE_ITEM == item->property("z_class_name").toString())
        {//如果是组合控件
           QObjectList itemschild = item->children();
           QWidget * childItem = nullptr;
           foreach (auto itemObj, itemschild)
           {
              childItem = qobject_cast<QWidget*>(itemObj);
              if(childItem)
              {
                  childItem->setParent(this);
                  childItem->show();
                  m_itemList.insert(m_itemList.indexOf(item),childItem);
                  childItem->stackUnder(item);

                  childItem->move(item->pos().x()+childItem->pos().x(),
                                         item->pos().y()+childItem->pos().y());
              }
           }
           m_itemList.removeOne(item);
           item->deleteLater();
        }
    }
}

void UWidgetScene::ItemAlignUp()
{
    QList<QWidget *> itemsList = getOptItems();
    //找到对齐标准线，最小的y坐标
    int minY = 0xffff;
    foreach (auto item, itemsList) {
        if(minY > item->pos().y())
        {
            minY = item->pos().y();
        }
    }
    //移动
    foreach (auto item, itemsList) {
        item->move(item->pos().x(),minY);
    }
}

void UWidgetScene::ItemAlignDown()
{
    QList<QWidget *> itemsList = getOptItems();
    //找到对齐标准线，最大的y坐标
    int maxY = 0x0;
    int bottom;;
    foreach (auto item, itemsList) {
        bottom = item->pos().y()+item->height();
        if(maxY < bottom)
        {
            maxY = bottom;
        }
    }
    //移动
    foreach (auto item, itemsList) {
        item->move(item->pos().x(),maxY-item->height());
    }
}

void UWidgetScene::ItemAlignLeft()
{
    QList<QWidget *> itemsList = getOptItems();
    //找到对齐标准线
    int minX = 0xffff;
    foreach (auto item, itemsList) {
        if(minX > item->pos().x())
        {
            minX = item->pos().x();
        }
    }
    //移动
    foreach (auto item, itemsList) {
        item->move(minX,item->pos().y());
    }
}

void UWidgetScene::ItemAlignRight()
{
    QList<QWidget *> itemsList = getOptItems();
    //找到对齐标准线
    int maxX = 0x0;
    int right;
    foreach (auto item, itemsList) {
        right = item->pos().x() + item->width();
        if(maxX < right)
        {
            maxX = right;
        }
    }
    //移动
    foreach (auto item, itemsList) {
        item->move(maxX-item->width(),item->pos().y());
    }
}

void UWidgetScene::ItemAlignCenterVer()
{
    QList<QWidget *> itemsList = getOptItems();
    //最上边的控件作为基准
    int ver = 0;
    int minY = 0xffff;
    foreach (auto item, itemsList) {
        if(item->y() < minY)
        {
            minY = item->y();
            ver = item->pos().x() + item->width()/2;
        }
    }
    //移动
    foreach (auto item, itemsList) {
        item->move(ver - item->width()/2,item->pos().y());
    }
}

void UWidgetScene::ItemAlignCenterHor()
{
    QList<QWidget *> itemsList = getOptItems();
    //最左边的控件作为基准
    int ver = 0;
    int minX = 0xffff;
    foreach (auto item, itemsList) {
        if(item->x() < minX)
        {
            minX = item->x();
            ver = item->pos().y() + item->height()/2;
        }
    }
    //移动
    foreach (auto item, itemsList) {
        item->move(item->pos().x(),ver - item->height()/2);
    }
}

void UWidgetScene::ItemIsometricVer()
{
    //垂直等距
    QList<QWidget *> itemsList = getOptItems();
    if(2 < itemsList.size())
    {
        //最上边的两个控件作为基准距离
        //先按y坐标大小排序
        std::sort(itemsList.begin(),itemsList.end(),[](QWidget * a, QWidget * b) {
            return (a->pos().y() < b->pos().y());
        });
        int distance = itemsList.at(1)->y() - itemsList.at(0)->y() - itemsList.at(0)->height();
        if(distance >= 0)
        {
            for(int i = 2; i < itemsList.size();++i)
            {
                itemsList.at(i)->move(itemsList.at(i)->pos().x(),
                                      itemsList.at(i-1)->y()+itemsList.at(i-1)->height() + distance);
            }
        }
    }
}

void UWidgetScene::ItemIsometricHor()
{
    //水平等距
    QList<QWidget *> itemsList = getOptItems();
    if(2 < itemsList.size())
    {
        //最左边的两个控件作为基准距离
        //先按x坐标大小排序
        std::sort(itemsList.begin(),itemsList.end(),[](QWidget * a, QWidget * b) {
            return (a->pos().x() < b->pos().x());
        });
        int distance = itemsList.at(1)->x() - itemsList.at(0)->x() - itemsList.at(0)->width();
        if(distance >= 0)
        {
            for(int i = 2; i < itemsList.size();++i)
            {
                itemsList.at(i)->move(itemsList.at(i-1)->x()+itemsList.at(i-1)->width() + distance,
                                      itemsList.at(i)->pos().y());
            }
        }
    }
}

void UWidgetScene::ItemEqualWidth()
{
    //等宽
    QList<QWidget *> itemsList = getOptItems();
    //平均值宽度
    int totalW = 0;
    int average = 0;
    foreach (auto item, itemsList) {
        totalW += item->width();
    }
    average = totalW/itemsList.size();
    foreach (auto item, itemsList) {
        item->setGeometry(item->pos().x(),item->pos().y(),average,item->height());
    }
}

void UWidgetScene::ItemEqualHeight()
{
    //等高
    QList<QWidget *> itemsList = getOptItems();
    //平均值
    int totalH = 0;
    int average = 0;
    foreach (auto item, itemsList) {
        totalH += item->height();
    }
    average = totalH/itemsList.size();
    foreach (auto item, itemsList) {
        item->setGeometry(item->pos().x(),item->pos().y(),item->width(),average);
    }
}

void UWidgetScene::ItemEqualWH()
{
    //等宽高
    ItemEqualWidth();
    ItemEqualHeight();
}

void UWidgetScene::ItemSaveCustomItem()
{
    QList<QWidget *> itemsList = getOptItems();
    if(1 == itemsList.size())
    {
       if(COMBINE_ITEM == itemsList.first()->property("z_class_name").toString())
       {
           QString file = QFileDialog::getSaveFileName(this,"Open",QApplication::applicationDirPath()+COMBINE_ITEM_PATH,"Files (*.item)");
           if (file.isNull())
           {
               return;
           }
           QStringList sList = file.split('/');
           QString fileName = sList.last().split('.').first();
           sList.removeLast();
           QString filePath = sList.join("/")+"/";

           //保存图
           itemsList.first()->grab().save(filePath+fileName+".png","PNG");

           //保存控件配置文件
           QJsonArray itemObjArray;
           saveItemsFromJson(itemObjArray,itemsList);
           if(1 == itemObjArray.size())
           {
               QJsonDocument document;
               document.setObject(itemObjArray.at(0).toObject());
               QFile file(filePath+fileName+".item");
               if(file.open(QIODevice::WriteOnly))
               {
                  file.write(document.toJson());
                  file.close();
               }
           }
       }
    }
}

void UWidgetScene::SceneProEdit()
{
    QList<QWidget*> itemlist = selectedItems();
    if(1 == itemlist.size())
    {//控件属性
        interface_item* item = dynamic_cast<interface_item*>(itemlist.at(0));
        if(item)
        {
            item->update_result(ITEM_SET_FLAG(0,Item_Update_Flag_ProEdit));
        }
    }
    else
    {//画面属性
        //获取json数据
        UScenePropertyEditDlg editDlg(this);
        editDlg.InitUI();
        editDlg.RefreshUI(m_sceneObj);
        if(QDialog::Accepted == editDlg.exec())
        {
            m_sceneObj = editDlg.m_sceneObj;
            initScenePropertyFromObj();
            SetChangeFlag();
        }
    }
}

bool UWidgetScene::addItem(QWidget *child, QPoint pos, QSize size)
{
    //属性
    child->setGeometry(pos.x(),pos.y(),size.width(),size.height());
    child->setVisible(true);

    interface_item *item = dynamic_cast<interface_item*>(child);
    if(item)
    {//设置控件的模式
        if(m_modeFlag)
        {
            item->update_result(0x60);
        }
        else
        {
            item->update_result(0x20);
        }
        item->initItem(m_view->m_mainW->m_comm,
                       m_view->m_mainW->m_gallery,
                       m_view->m_mainW->m_varTable);
    }
    if(-1 == m_itemList.indexOf(child))
    {
        m_itemList.push_back(child);
        return true;
    }
    return false;
}

bool UWidgetScene::addItem(QWidget *child)
{
    interface_item *item = dynamic_cast<interface_item*>(child);
    if(item)
    {//设置控件的模式
        if(m_modeFlag)
        {
            item->update_result(0x60);
        }
        else
        {
            item->update_result(0x20);
        }
        item->initItem(m_view->m_mainW->m_comm,
                       m_view->m_mainW->m_gallery,
                       m_view->m_mainW->m_varTable);
    }
    if(-1 == m_itemList.indexOf(child))
    {
        m_itemList.push_back(child);
        return true;
    }
    return false;
}

QWidget *UWidgetScene::cloneOfItem(QWidget *item,QWidget *parent)
{
    //创建
    QString sClassName = item->property("z_class_name").toString();
    QWidget * newitem = itemsFactory::GetInstance()->createItemFromName(sClassName,
                                                                        parent==nullptr?qobject_cast<QWidget*>(item->parent()):parent);
    //属性配置
    QJsonObject obj;
    GetItemPro(item,obj);
    if(obj.isEmpty())
    {
        itemsFactory::GetInstance()->createItemObjFromName(sClassName,newitem);
    }
    else
    {
        itemsFactory::GetInstance()->createItemObjFromName(sClassName,newitem,&obj);
    }

    //属性
    initItem(newitem,sClassName,item->pos().x(),item->pos().y(),item->width(),item->height());

    //处理组合控件
    if( COMBINE_ITEM == sClassName)
    {
        QObjectList childList = item->children();
        //转换
        QList<QWidget*> itemlistCombine;
        foreach (auto itemchild, childList)
        {
           itemlistCombine.push_back(qobject_cast<QWidget*>(itemchild));
        }
        foreach (auto itemchild, itemlistCombine) {
           QWidget *newI = cloneOfItem(itemchild,newitem);
           //设置组合时的长宽用来后续尺寸计算
           newI->setProperty(COMBINE_ITEM_X,newI->pos().x());
           newI->setProperty(COMBINE_ITEM_Y,newI->pos().y());
           newI->setProperty(COMBINE_ITEM_W,newI->width());
           newI->setProperty(COMBINE_ITEM_H,newI->height());
        }
        //设置组合时的长宽用来后续尺寸计算
        newitem->setProperty(COMBINE_ITEM_PARENT_W,newitem->width());
        newitem->setProperty(COMBINE_ITEM_PARENT_H,newitem->height());
    }

    return newitem;
}

QWidget *UWidgetScene::itemAt(const QPoint &pos)
{
    //return childAt(pos.toPoint());
    //后续优化时间复杂度
    for (int i = m_itemList.size()-1; i >= 0 ; --i)
    {
        QRect rect(m_itemList.at(i)->pos(),m_itemList.at(i)->size());
        if(rect.contains(pos))
        {
            return m_itemList.at(i);
        }
    }
    return nullptr;
}

QList<QWidget *> UWidgetScene::setSelectitemsAtRect(const QRect &rect, bool flag)
{
    QList<QWidget *> itemsList;
    foreach (auto item, m_itemList)
    {
        if(rect.contains(QRect(item->pos(),item->size())))
        {
            itemsList.push_back(item);
            setSelected(item,flag);
        }
    }
    return itemsList;
}

QList<QWidget *> UWidgetScene::items()
{
    return m_itemList;
}

void UWidgetScene::setSelected(QWidget *item, bool flag)
{
    if(item->property("isSelected").toBool() != flag)
    {
        item->setProperty("isSelected",flag);
        item->update();
    }
}

bool UWidgetScene::isSelected(QWidget *item)
{
    if(item)
        return item->property("isSelected").toBool();
    else
        return false;
}

QList<QWidget *> UWidgetScene::selectedItems()
{
    QList<QWidget*> itemList;
    for(int i = 0; i < m_itemList.size();i++)
    {
        if(m_itemList.at(i)->property("isSelected").toBool())
        {
            itemList.push_front(m_itemList.at(i));
        }
    }
    return itemList;
}

QList<QWidget *> UWidgetScene::getOptItems()
{
    QList<QWidget*> itemList;
    for(int i = 0; i < m_itemList.size();i++)
    {
        if(m_itemList.at(i)->property("isSelected").toBool() && !m_itemList.at(i)->property("isLocked").toBool())
        {
            itemList.push_front(m_itemList.at(i));
        }
    }
    return itemList;
}

void UWidgetScene::setCreateMode(const QString& itemName)
{
    m_currentPolicy = m_view->RectCreateUIInput;
    m_creatPolicyItemName = itemName;
}

void UWidgetScene::setSelectMode()
{
    m_currentPolicy = m_view->selectUIInput;
}

void UWidgetScene::setEditMode()
{
   m_currentPolicy = m_view->RectEditUIInput;
}

void UWidgetScene::setUIMode(IUIInputPolicy *policy)
{
    m_currentPolicy = policy;
}

void UWidgetScene::setCursorType(Qt::CursorShape curShape)
{
    setCursor(curShape);
}

Qt::CursorShape UWidgetScene::getCursorShape()
{
    return cursor().shape();
}

void UWidgetScene::SetChangeFlag()
{
    if(m_view)
    {
        m_view->SetChangeFlag(this);
    }
}

interface_gallery *UWidgetScene::GetGallery()
{
    return m_view->m_mainW->m_gallery.get();
}

void UWidgetScene::cancelSelect()
{
    for(int i = 0; i < m_itemList.size(); i++)
    {
        setSelected(m_itemList.at(i),false);
    }
}

void UWidgetScene::AssociateAddressAndItem()
{
    if(!m_modeFlag)
    {
        if(0 == m_addrsInfo.size())
        {//如果该画面未采集过地址
            std::vector<Protocol::AddrInfoForRW> addrlist;
            for(int i= 0;i< m_itemList.size();i++)
            {
                QStringList list;
                QList<Protocol::AddrInfoForRW> addrlist;
                interface_item * item = dynamic_cast<interface_item *>(m_itemList.at(i));
                if(item)
                {
                    item->update_addr(addrlist);
                }
                m_addrsInfo.append(addrlist);
            }
        }
        m_view->m_mainW->m_comm->SetCycReadAddrinfo("link4",m_addrsInfo.toVector().toStdVector(),Protocol::Cover);
        m_view->m_mainW->m_comm->SetAddrValueChangeCB("link4",m_addrsInfo.toVector().toStdVector(),Protocol::Cover);
    }
}

//int g_is_offline_simulation = 0;//0-在线 1-离线
void UWidgetScene::mousePressEvent(QMouseEvent *evt)
{
    //模式判断
    if(m_modeFlag)
    {//编辑模式
        if (m_currentPolicy)
        {
            while(!m_currentPolicy->mousePress(evt)){}
        }
        update();

        //处理状态栏
        QList<QWidget*> lists = selectedItems();
        if(1 == lists.size())
        {
            m_statusBar->leftEdit->setText(QString::number( lists.at(0)->pos().x() ));
            m_statusBar->topEdit->setText(QString::number( lists.at(0)->pos().y() ));
            m_statusBar->widthEdit->setText(QString::number( lists.at(0)->width() ));
            m_statusBar->heightEdit->setText(QString::number( lists.at(0)->height() ));
        }
    }
    QWidget::mousePressEvent(evt);
}

void UWidgetScene::mouseMoveEvent(QMouseEvent *evt)
{
    //模式判断
    if(m_modeFlag)
    {//编辑模式
        QPointF pos = evt->pos();
        //status bar
        if(m_statusBar)
        {
            m_statusBar->showMessage(QString("x=%1,y=%2").arg(pos.x()).arg(pos.y()));
        }
        if (m_currentPolicy)
        {
            m_currentPolicy->mouseMove(evt);
        }
        update();
    }
    QWidget::mouseMoveEvent(evt);
}

void UWidgetScene::mouseReleaseEvent(QMouseEvent *evt)
{
    //模式判断
    if(m_modeFlag)
    {//编辑模式
        if (m_currentPolicy)
        {
            m_currentPolicy->mouseRelease(evt);
        }
        update();
    }
    QWidget::mouseReleaseEvent(evt);
}

void UWidgetScene::keyPressEvent(QKeyEvent *event)
{
    if(m_modeFlag)
    {//编辑模式
        if (m_currentPolicy)
        {
            m_currentPolicy->keyPressEvent(event);
        }
    }
    QWidget::keyPressEvent(event);
}

void UWidgetScene::keyReleaseEvent(QKeyEvent *event)
{
    QWidget::keyReleaseEvent(event);
}

QStringList UWidgetScene::compile()
{
    QStringList errorList;
    return errorList;
}

void UWidgetScene::dealCompileError(QStringList &errInfoList)
{
    if( 1 == errInfoList.size())
    {
        //画面本身属性编译报错
    }
    else if(2 == errInfoList.size())
    {
       //控件编译报错
       //
    }
}

