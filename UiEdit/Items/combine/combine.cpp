#include <QPen>
#include <QPainter>
#include <QRectF>
#include <QFontDatabase>
#include <QApplication>
#include <QJsonObject>
#include "combine.h"
#include <QDebug>

interface_item *createItem(QWidget *parent)
{
    return new combine(parent);
}

void getItemName(std::string &sName)
{
    sName = "combine";
}

void  item_object_create(const std::string &name, QWidget * item, QJsonObject *object)
{
    Q_UNUSED(name);
    item_base::itemObjCreate(item,object);
    if(item && object)
    {
        combine* bitButtonItem = qobject_cast<combine*>(item);
        if(bitButtonItem)
        {
            bitButtonItem->SetPro(*object);
        }
    }
}

void item_object_save(const std::string &name, QWidget *item, QJsonObject *object)
{
    if(item && object)
    {
        combine* Item = qobject_cast<combine*>(item);
        if(Item)
        {
            Item->GetPro(*object);
        }
        item_base::itemObjSave(name,item,object);
    }
}

combine::combine(QWidget *parent):item_base(parent)
{
}

void combine::initWidget()
{
   //m_w = new QPushButton(this);
   /*
   m_w->setStyleSheet(QString("QPushButton{background-color:rgba(200,200,200,200);\
                                color:rgb(0,0,0);\
                                border-radius:20px;\
                                border: 3px outset rgb(128,128,128);\
                                font:bold 30px;\
                                }\
                                QPushButton:pressed{\
                                background-color:rgb(255,0,0);\
                                color:rgb(0,255，255);\
  }") );
  */
}

void combine::itemPaintEvent(QPaintEvent *event)
{
#if 0
    //组合控件的边框和背景绘制
    (void)event;
    QPainter p(this);
    //p.setRenderHint(QPainter::Antialiasing);//抗锯齿
    p.setPen(QPen(QBrush(QColor(180,180,180)),5));
    p.drawRoundedRect(0, 0, width() -1, height() -1,15,15); //绘制边框
#endif
}

bool combine::update_result(int flag, void *reslut)
{
    if( ITEM_GET_FLAG(flag,Item_Update_Flag_ModeChange) )
    {//处理模式切换
       m_modeType  = (flag >> 6)& 0x1;
       itemProChangeOfMode();
    }

    if(ITEM_GET_FLAG(flag,Item_Update_Flag_GetPro))
    {//获取控件属性
        (*(reinterpret_cast<QJsonObject*>(reslut))) = m_itemObj;
    }

    return false;
}

bool combine::update_addr(QList<Protocol::AddrInfoForRW> &addrList)
{
//    Protocol::AddrInfoForRW addr1;
//    addr1.linkName = "link4";
//    addr1.reg = "0x";
//    addr1.index = 1;
//    addr1.len = 1;
//    addr1.dataType = Protocol::bit;
//    addr1.callback =  std::bind(&combine::addrChangeCB,this,std::placeholders::_1,std::placeholders::_2);
//    addrList.push_back(addr1);
    return true;
}

void combine::addrChangeCB(const Protocol::AddrInfoForRW &, int32_t code)
{
//    Protocol::AddrInfoForRW addr1;
//    addr1.linkName = "link4";
//    addr1.reg = "0x";
//    addr1.index = 1;
//    addr1.len = 1;
//    addr1.dataType = Protocol::bit;

//    if(m_comm)
//    {
//        m_comm->GetValue(addr1,Protocol::CurrentValue);
//        if(addr1.value.index() == Protocol::bit)
//        {
//            m_curStatus = std::get<Protocol::bit>(addr1.value);
//            itemUpdate();
//        }
//    }
}



