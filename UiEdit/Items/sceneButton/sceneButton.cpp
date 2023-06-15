#include <QPen>
#include <QPainter>
#include <QRectF>
#include <QFontDatabase>
#include <QJsonObject>
#include "sceneButton.h"
#include "sceneButtonProDlg.h"
#include <QDebug>

#include "../../UiEdit/Gallery/interface_gallery.h"
#include "../../UiEdit/VariableTable/interface_variable_table.h"

#include "IApp.h"
#include "extend_func_head.h"

interface_item *createItem(QWidget *parent)
{
    return new USceneButton(parent);
}

void getItemName(std::string &sName)
{
    sName = "Scene Button";
}

QAction *getItemAction(const std::string &sName)
{
    if("Scene Button" == sName)
    {
        static QAction action(qApp);
        action.setIcon(QIcon(QString(":/Scene Button.png")));
        action.setText(QObject::tr("Scene Button"));
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
        USceneButton* bitButtonItem = qobject_cast<USceneButton*>(item);
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
        USceneButton* bitButtonItem = qobject_cast<USceneButton*>(item);
        if(bitButtonItem)
        {
            bitButtonItem->GetPro(*object);
        }
        item_base::itemObjSave(name,item,object);
    }
}

USceneButton::USceneButton(QWidget *parent):item_base(parent)
{
}

void USceneButton::initWidget()
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

void USceneButton::itemPaintEvent(QPaintEvent *event)
{
    (void)event;
    QPainter p(this);
    p.setPen(QPen(QBrush(QColor(180,180,180)),5));
    p.setBrush(QBrush(QColor(180,180,180)));
    p.drawRoundedRect(0,0,width(),height(),15,15,Qt::AbsoluteSize); //绘制边框
    //文本
    p.setPen(QPen(QBrush(QColor(0,0,0)),5));
    p.drawText(QRectF(0,0,width(),height()),m_itemObj["text"].toString(),QTextOption(Qt::AlignCenter));
}

bool USceneButton::update_result(int flag)
{
    if( ITEM_GET_FLAG(flag,Item_Update_Flag_ModeChange) )
    {//处理模式切换
       m_modeType  = ITEM_GET_FLAG(flag,Item_Update_Flag_ModeChange_V);
       itemProChangeOfMode();
       //属性窗口创建
       if(m_modeType && !m_proEditDlg)
       {
           m_proEditDlg = new sceneButtonProDlg(this,m_mainWin);
           m_proEditDlg->InitUI();
       }
    }
    if(ITEM_GET_FLAG(flag,Item_Update_Flag_Refresh))
    {//主动刷新
        return true;
    }
    if(ITEM_GET_FLAG(flag,Item_Update_Flag_SceneOpen))
    {//打开画面
        if(m_gallery)
        {//从图库更新图片
            //m_gallery->GetImgFromPath(m_itemObj["on_img"].toString("on.png").toLocal8Bit().data(),m_on);
        }
    }
    if(ITEM_GET_FLAG(flag,Item_Update_Flag_ProEdit))
    {//属性编辑
       showProEditDlg();
    }
    return false;
}

bool USceneButton::update_addr(QList<Protocol::AddrInfoForRW> &addrList)
{
#if 0
    Protocol::AddrInfoForRW addr1;
    if(m_variable)
    {
        m_variable->GetAddrinfoFromVarName(m_itemObj["var"].toString().toLocal8Bit().data(),addr1);
    }
    addr1.callback =  std::bind(&USceneButton::addrChangeCB,this,std::placeholders::_1,std::placeholders::_2);
    addrList.push_back(addr1);
#endif
    return true;
}

void USceneButton::addrChangeCB(const Protocol::AddrInfoForRW &, int32_t code)
{
#if 0
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
#endif
}

void USceneButton::mousePressEvent(QMouseEvent *event)
{
    if(!m_modeType)
    {
        IAPP::GetIntance()->InvokeMethod(Extend_View_SwitchScene,Z_ARG(const std::string &,
                                                                       m_itemObj["scene"].toString().toLocal8Bit().data()));
    }
    QWidget::mousePressEvent(event);
}



