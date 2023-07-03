#include <QPen>
#include <QPainter>
#include <QRectF>
#include <QFontDatabase>
#include <QJsonObject>
#include "bitButton.h"
#include "bitButtonProDlg.h"
#include <QDebug>
#include "../../UiEdit/Gallery/interface_gallery.h"
#include "../../UiEdit/VariableTable/interface_variable_table.h"

interface_item *createItem(QWidget *parent)
{
    return new UBitButton(parent);
}

void getItemName(std::string &sName)
{
    sName = "Bit Button";
}

QAction *getItemAction(const std::string &sName)
{
    if("Bit Button" == sName)
    {
        static QAction action(qApp);
        action.setIcon(QIcon(QString(":/Bit Button.png")));
        action.setText(QObject::tr("Bit Button"));
        return &action;
    }
    return nullptr;
}

void  item_object_create(const std::string &name, QWidget * item, QJsonObject *object)
{
    Q_UNUSED(name);
    item_base::itemObjCreate(item,object);
    if(item && object)
    {
        UBitButton* bitButtonItem = qobject_cast<UBitButton*>(item);
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
        UBitButton* bitButtonItem = qobject_cast<UBitButton*>(item);
        if(bitButtonItem)
        {
            bitButtonItem->GetPro(*object);
        }
        item_base::itemObjSave(name,item,object);
    }
}

UBitButton::UBitButton(QWidget *parent):item_base(parent)
{
}

void UBitButton::initWidget()
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
    if(m_gallery)
    {//从图库更新图片
        m_gallery->GetImgFromPath(m_itemObj["on_img"].toString("on.png").toLocal8Bit().data(),m_on);
        m_gallery->GetImgFromPath(m_itemObj["off_img"].toString("off.png").toLocal8Bit().data(),m_off);
    }
}

void UBitButton::itemPaintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    //painter.setRenderHint(QPainter::Antialiasing);//抗锯齿
    if(m_on && m_off)
    {
        if(m_curStatus)
        {
            painter.drawImage(rect(),*m_on);
        }
        else
        {
            painter.drawImage(rect(),*m_off);
        }
    }
}

bool UBitButton::update_result(int flag, void *reslut)
{
    if( ITEM_GET_FLAG(flag,Item_Update_Flag_ModeChange) )
    {//处理模式切换
       m_modeType  = ITEM_GET_FLAG(flag,Item_Update_Flag_ModeChange_V);
       itemProChangeOfMode();
       //属性窗口创建
       if(m_modeType && !m_proEditDlg)
       {
           m_proEditDlg = new bitButtonProDlg(this,m_mainWin);
           m_proEditDlg->InitUI();
       }
    }
    if(ITEM_GET_FLAG(flag,Item_Update_Flag_Refresh))
    {//主动刷新
        if(m_gallery)
        {//从图库更新图片
            m_gallery->GetImgFromPath(m_itemObj["on_img"].toString("on.png").toLocal8Bit().data(),m_on);
            m_gallery->GetImgFromPath(m_itemObj["off_img"].toString("off.png").toLocal8Bit().data(),m_off);
        }
        return true;
    }
    if(ITEM_GET_FLAG(flag,Item_Update_Flag_SceneOpen))
    {//打开画面
        if(m_gallery)
        {//从图库更新图片
            m_gallery->GetImgFromPath(m_itemObj["on_img"].toString("on.png").toLocal8Bit().data(),m_on);
            m_gallery->GetImgFromPath(m_itemObj["off_img"].toString("off.png").toLocal8Bit().data(),m_off);
        }
    }
    if(ITEM_GET_FLAG(flag,Item_Update_Flag_ProEdit))
    {//属性编辑
       showProEditDlg();
    }

    if(ITEM_GET_FLAG(flag,Item_Update_Flag_GetPro))
    {//获取控件属性
        (*(reinterpret_cast<QJsonObject*>(reslut))) = m_itemObj;
    }

    return false;
}

bool UBitButton::update_addr(QList<Protocol::AddrInfoForRW> &addrList)
{
    Protocol::AddrInfoForRW addr1;
    if(m_variable)
    {
        m_variable->GetAddrinfoFromVarName(m_itemObj["var"].toString().toLocal8Bit().data(),addr1);
    }
    addr1.callback =  std::bind(&UBitButton::addrChangeCB,this,std::placeholders::_1,std::placeholders::_2);
    addrList.push_back(addr1);
    return true;
}

void UBitButton::addrChangeCB(const Protocol::AddrInfoForRW &, int32_t code)
{
    Protocol::AddrInfoForRW addr1;
    if(m_variable)
    {
        m_variable->GetAddrinfoFromVarName(m_itemObj["var"].toString().toLocal8Bit().data(),addr1);
    }

    if(m_comm)
    {
        m_comm->GetValue(addr1,Protocol::CurrentValue);
        if(addr1.value.index() == Protocol::bit)
        {
            m_curStatus = std::get<Protocol::bit>(addr1.value);
            itemUpdate();
        }
    }
}



