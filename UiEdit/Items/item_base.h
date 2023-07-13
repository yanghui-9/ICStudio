#ifndef ITEM_BASE_H
#define ITEM_BASE_H
#include <QPainter>
#include <QApplication>
#include <QDebug>
#include  "interface_item.h"
#include "ItemProEditDlgBase.h"
#include "interface_comm.h"


#if defined(_ITEM)
#  define _API_ITEM extern "C" Q_DECL_EXPORT
#else
#  define _API_ITEM extern "C" Q_DECL_IMPORT
#endif

_API_ITEM interface_item *createItem(QWidget* parent);
_API_ITEM void  getItemName(std::string &name);
_API_ITEM QAction *getItemAction(const std::string &sName);
_API_ITEM void item_object_create(const std::string &name, QWidget *item, QJsonObject *object);
_API_ITEM void item_object_save(const std::string &name, QWidget *item, QJsonObject *object);

class QMainWindow;
class item_base : public interface_item
{
    Q_OBJECT
public:
    item_base(QWidget *parent = nullptr):interface_item(parent){}
    virtual ~item_base(){
        if(m_proEditDlg)
        {
           delete m_proEditDlg;
        }
    }


    //子类需要实现接口初始化
    //初始化widget
    virtual void initWidget(){}
    //自定义绘制部分
    virtual void itemPaintEvent(QPaintEvent *event){Q_UNUSED(event);}

protected:
    virtual void initItem(std::shared_ptr<interface_comm> &comm,
                          std::shared_ptr<interface_gallery> &gallery,
                          std::shared_ptr<interface_variable_table> &variable){
        m_comm = comm;
        m_gallery = gallery;
        m_variable = variable;
        setMouseTracking(true);
        setWidgetPro();
    }
    void setWidgetPro(){
        initWidget();
        itemProChangeOfMode();
    }
    //更新控件
    virtual void itemUpdate()
    {
       QMetaObject::invokeMethod(this,"update",Qt::QueuedConnection);
    }

    //控件模式切换，属性修改
    void itemProChangeOfMode()
    {
        if(m_w)
        {
            m_w->setAttribute(Qt::WA_TransparentForMouseEvents,m_modeType);//事件往父窗口传递;
            m_w->setMouseTracking(m_modeType);
        }
    }
    //属性编辑框
    void showProEditDlg()
    {
        if(m_proEditDlg)
        {
            m_proEditDlg->RefreshUI();
            if(QDialog::Accepted == m_proEditDlg->exec())
            {
                if(update_result(ITEM_SET_FLAG(0,Item_Update_Flag_Refresh)))
                {
                    itemUpdate();
                }
            }
        }
    }

    //双击事件弹出属性框
    void mouseDoubleClickEvent(QMouseEvent *event)
    {
        Q_UNUSED(event);
        showProEditDlg();
    }

public:
    //控件属性
    void GetPro(QJsonObject &obj){
        obj = m_itemObj;
    }
    void SetPro(const QJsonObject &obj){
        m_itemObj = obj;
    }
    //通用控件创建时属性处理
    static void itemObjCreate(QWidget * item, QJsonObject *object){
        if(!item) return;
        if(!object){
            item->setGeometry(0,0,1,1);
        }
        else{
            item->setGeometry(object->value("z_x").toInt(0),object->value("z_y").toInt(0),
                              object->value("z_w").toInt(1),object->value("z_h").toInt(1));
        }
    }
    //通用控件保存时属性处理
    static void itemObjSave(const std::string &name, QWidget *item, QJsonObject *object){
        (*object)["z_w"] = item->width();
        (*object)["z_h"] = item->height();
        (*object)["z_x"] = item->pos().x();
        (*object)["z_y"] = item->pos().y();
        (*object)["z_class_name"] = QString::fromStdString( name );
    }

protected:
    void paintEvent(QPaintEvent *event){
        Q_UNUSED(event);
        //处理自定义绘制
        itemPaintEvent(event);
        if(m_modeType)
        {//编辑模式下
            paintSelectRect();//判断是否选中，绘制选择边线
        }
        if(m_w)
        {//如果有绑定控件的情况下
            m_w->resize(width()-20,height()-20);
            m_w->move(10,10);
        }
    }
    void paintSelectRect(){
        if(property("isSelected").toBool())
        {
            QPainter painter(this);
            QRectF normalrectF = rect();
            normalrectF.setX(normalrectF.x());
            normalrectF.setY(normalrectF.y());
            normalrectF.setWidth(normalrectF.width());
            normalrectF.setHeight(normalrectF.height());
            painter.setPen(QPen(Qt::blue, 8, Qt::DashLine));
            painter.drawRect(static_cast<int>(normalrectF.x()),static_cast<int>(normalrectF.y())
                             ,static_cast<int>(normalrectF.width()),static_cast<int>(normalrectF.height()));
        }
    }

public:
    //外设，图库，变量表
    std::shared_ptr<interface_comm> m_comm;
    std::shared_ptr<interface_gallery> m_gallery;
    std::shared_ptr<interface_variable_table> m_variable;

protected:
    //qt 控件封装使用
    QWidget * m_w = nullptr;

    //true默认编辑模式
    bool m_modeType = true;

    //属性对话框（后续优化-每一类控件共用同一个编辑框对象）
    ItemProEditDlgBase * m_proEditDlg = nullptr;
    QJsonObject m_itemObj;

    //弹出属性框父窗口
    QWidget * m_mainWin = nullptr;
};
#endif // ITEM_BASE_H
