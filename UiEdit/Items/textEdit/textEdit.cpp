#include <QPen>
#include <QPainter>
#include <QRectF>
#include <QFontDatabase>
#include <QJsonObject>
#include <QTextEdit>
#include "textEdit.h"
#include "textEditProDlg.h"
#include <QDebug>

#include "../../UiEdit/Gallery/interface_gallery.h"
#include "../../UiEdit/VariableTable/interface_variable_table.h"

#include "IApp.h"
#include "extend_func_head.h"

interface_item *createItem(QWidget *parent)
{
    return new textEdit(parent);
}

void getItemName(std::string &sName)
{
    sName = "TextEdit";
}

QAction *getItemAction(const std::string &sName)
{
    if("TextEdit" == sName)
    {
        static QAction action(qApp);
        action.setIcon(QIcon(QString(":/TextEdit.png")));
        action.setText(QObject::tr("TextEdit"));
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
        textEdit* bitButtonItem = qobject_cast<textEdit*>(item);
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
        textEdit* bitButtonItem = qobject_cast<textEdit*>(item);
        if(bitButtonItem)
        {
            bitButtonItem->GetPro(*object);
        }
        item_base::itemObjSave(name,item,object);
    }
}

textEdit::textEdit(QWidget *parent):item_base(parent)
{
}

void textEdit::initWidget()
{
    m_w = new QTextEdit(this);
    m_w->show();
    itemUpdate();
    //m_w->resize(50,50);
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

bool textEdit::update_result(int flag, void *reslut)
{
    bool bRet = true;
    if( ITEM_GET_FLAG(flag,Item_Update_Flag_ModeChange) )
    {//处理模式切换
       m_modeType  = ITEM_GET_FLAG(flag,Item_Update_Flag_ModeChange_V);
       itemProChangeOfMode();
       //属性窗口创建
       if(m_modeType && !m_proEditDlg)
       {
           m_proEditDlg = new textEditProDlg(this,m_mainWin);
           m_proEditDlg->InitUI();
       }
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

    if(ITEM_GET_FLAG(flag,Item_Update_Flag_GetPro))
    {//获取控件属性
        (*(reinterpret_cast<QJsonObject*>(reslut))) = m_itemObj;
    }
    return bRet;
}

bool textEdit::update_addr(QList<Protocol::AddrInfoForRW> &addrList)
{
    if(m_itemObj["var"].toString().isEmpty())
    {
        return false;
    }
    Protocol::AddrInfoForRW addr1;
    if(m_variable)
    {
        m_variable->GetAddrinfoFromVarName(m_itemObj["var"].toString().toLocal8Bit().data(),addr1);
    }
    addr1.callback =  std::bind(&textEdit::addrChangeCB,this,std::placeholders::_1,std::placeholders::_2);
    addrList.push_back(addr1);
    return true;
}

void textEdit::addrChangeCB(const Protocol::AddrInfoForRW &, int32_t code)
{
    (void)code;
    Protocol::AddrInfoForRW addr1;
    if(m_variable)
    {
        m_variable->GetAddrinfoFromVarName(m_itemObj["var"].toString().toLocal8Bit().data(),addr1);
    }

    if(m_comm)
    {
        m_comm->GetValue(addr1,Protocol::CurrentValue);
        if(addr1.value.index() == Protocol::bit16)
        {
            //m_curStatus = std::get<Protocol::bit>(addr1.value);
            m_text = QString::number( std::get<Protocol::bit16>(addr1.value) );
            itemUpdate();
        }
    }
}

void textEdit::itemUpdate()
{
    if(m_modeType)
    {
        (qobject_cast<QTextEdit*>(m_w))->setText(m_itemObj["text"].toString());
    }
    else
    {
        //(qobject_cast<QTextEdit*>(m_w))->setText(m_text);
        QMetaObject::invokeMethod(qobject_cast<QTextEdit*>(m_w),"setPlainText",Qt::QueuedConnection,Q_ARG(QString,m_text));
    }
}

void textEdit::mousePressEvent(QMouseEvent *event)
{
    //
    QWidget::mousePressEvent(event);
}



