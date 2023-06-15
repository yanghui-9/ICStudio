#include "RectEdit_UIInput.h"
#include <QtCore/qmath.h>
#include "qmath.h"
#include <QDebug>
#include "../Undo/UMoveUndo.h"
#include "../Undo/URectUndo.h"

RectEdit_UIInput::RectEdit_UIInput()
{
    m_selectedItem = nullptr;
}

// 获取组件有效的起始X坐标
double RectEdit_UIInput::getValid_X(double value)
{
    if (value < 0)
    {
        value = 0;
    }
    else if (value > mScreen->rect().width()-10)
    {
        value = mScreen->rect().width()-10;
    }

    return value;
}

// 获取组件有效的起始Y坐标
double RectEdit_UIInput::getValid_Y(double value)
{
    if (value < 0)
    {
        value = 0;
    }
    else if (value > mScreen->rect().height()-10)
    {
        value = mScreen->rect().height()-10;
    }

    return value;
}


/*virtual*/ bool RectEdit_UIInput::mousePress(QMouseEvent *evt )
{
    QPoint pos = evt->pos();

    m_PressPos = pos; //TODO：保存坐标，用于拖拽复制功能（2016-04-22）
    m_pressFlag = true;

    if( !mScreen )
    {
        return true;
    }

    // 获取鼠标所在位置的item
    QWidget * item = mScreen->itemAt(pos);

    //判断是否按下ctrl键
    if(Qt::ControlModifier == evt->modifiers() && item)
    {//按下了且当前按下位置有控件
        mScreen->setSelected(item);
    }

    //如果点击空白区域
    if(!item)
    {
        mScreen->cancelSelect();
        mScreen->setCursorType( Qt::ArrowCursor );
        m_pressFlag = false;
        mScreen->setSelectMode();
        return false;
    }
    else
    {
        if(!mScreen->isSelected(item))
        {
            m_mouseIndex = mouse_nor;
            mScreen->setCursor(Qt::SizeAllCursor);
        }
    }

    //点击到未选择的控件
    if( item && !mScreen->isSelected(item))
    {
        mScreen->cancelSelect();
        mScreen->setSelected(item);
    }

    //记录控件尺寸变化之前size
    m_oldRect = QRect(item->pos(),item->size());

    m_movePos = pos;
    return true;
}

/*virtual*/ bool RectEdit_UIInput::mouseMove(QMouseEvent *evt )
{
    QPoint pos = evt->pos();
    pos.setX(pos.x());
    pos.setY(pos.y());

    if( !m_pressFlag )//  && Qt::NoButton == QApplication::mouseButtons())
    {//鼠标未按下
        DealMouseType(pos);
    }
    else
    {
        QList <QWidget *> list1 = mScreen->getOptItems();
        if(0 == list1.size())
        {
            return false;
        }
        //记录移动距离
        QPointF dis = evt->pos() - m_movePos;
        //开始
        switch (m_mouseIndex) {
        case mouse_nor:
        {//移动
            if (qFabs(dis.x()) >= 5 || qFabs(dis.y()) >= 5)
            {
                for(int i = 0; i < list1.size() ; i++)
                {
                    //移动图形
                    doItemMove( dis, list1.at(i));
                }
                //更新移动值
                m_movePos = evt->pos();
            }
        }
            break;
        case Horizontal_Left:
        case Horizontal_Right:
        {
            if(1 == list1.size())
            {
                 // 水平调整处理 8
                doHorStretch(m_mouseIndex-Horizontal_Left, pos, list1.at(0) );
                m_movePos = evt->pos();
            }
        }
            break;
        case Vertical_Up:
        case Vertical_Down:
        {
            if(1 == list1.size())
            {
                // 垂直调整 9
                doVerStretch(m_mouseIndex-Vertical_Up, pos, list1.at(0) );
                m_movePos = evt->pos();
            }
        }
            break;
        case  Upper_Left:
        case  Lower_Right:
        {
            if(1 == list1.size())
            {
                // 沿对角线调整2 10
                doFDiagStretch(m_mouseIndex-Upper_Left, pos, list1.at(0) );
                m_movePos = evt->pos();
            }
        }
            break;
        case  Upper_Right:
        case  Lower_Left:
        {
            if(1 == list1.size())
            {
                // 沿对角线调整1 11
                doBDiagStretch( m_mouseIndex-Upper_Right, pos, list1.at(0) );
                m_movePos = evt->pos();
            }
        }
            break;
        }
        // 发送项目修改信号
        mScreen->SetChangeFlag();
    }

    return false;
}

/*virtual*/ bool RectEdit_UIInput::mouseRelease(QMouseEvent *evt )
{
    QList <QWidget *> listItems = mScreen->selectedItems();
    //判断是否需要切换为选择模式
    if(listItems.size() == 0)
    {
        // 鼠标按下的位置没有item或者按下的地方不是选中的item
        // 设置按下点为伸缩点处的鼠标形状
        // 获取对象
        mScreen->setCursorType( Qt::ArrowCursor );
        mScreen->setSelectMode();
        mScreen->cancelSelect();
    }
    //undo处理
    else if(m_pressFlag)
    {
       switch (m_mouseIndex) {
       case mouse_nor:
       {//移动
           if(m_PressPos != evt->pos())
           {
               mScreen->getUndoStack()->push(new UMoveUndo(m_PressPos,evt->pos(),listItems));
           }
       }
           break;
       case Horizontal_Left:
       case Horizontal_Right:
       case Vertical_Up:
       case Vertical_Down:
       case  Upper_Left:
       case  Lower_Right:
       case  Upper_Right:
       case  Lower_Left:
       {
           if(1 == listItems.size())
           {
               QRect newRect = QRect(listItems.at(0)->pos(),listItems.at(0)->size());
               if(newRect != m_oldRect)
               {
                   mScreen->getUndoStack()->push(new URectUndo(m_oldRect,
                                                               newRect,
                                                               listItems.at(0)));
               }
           }
       }
           break;
       }
    }

    m_movePos = evt->pos();
    m_pressFlag = false;

    return false;
}


/************************************************************
*函数名称：doHorStretch
*函数功能：水平伸缩操作
**************************************************************/
void RectEdit_UIInput::doHorStretch(int index, QPoint pos, QWidget *item )
{
    // 获取start属性值
    int start_X = item->pos().x();
    int start_Y = item->pos().y();

    // 获取size属性值
    int size_W = item->width();
    int size_H = item->height();

    if ( index == 0 )
    {
        // 设置start和size属性值
        int newsize_w = size_W+start_X - static_cast<int>( pos.x() );
        int newsize_h = size_H;
        item->setGeometry(pos.x(),
                          start_Y,
                          (qAbs(newsize_w)<1)?1:newsize_w,
                          (qAbs(newsize_h)<1)?1:newsize_h);
    }
    else if ( index == 1 )
    {
        // 设置start和size属性值
        int newsize_w = static_cast<int>(pos.x())- start_X;
        int newsize_h = size_H;
        item->setGeometry(start_X,
                          start_Y,
                          (qAbs(newsize_w)<1)?1:newsize_w,
                          (qAbs(newsize_h)<1)?1:newsize_h);
    }
    ItemMiniSize(item);
    //处理组合控件
    DealCombineItem(item);
    item->update();
}


/************************************************************
*函数名称：doVerStretch
*函数功能：垂直伸缩操作
**************************************************************/
void RectEdit_UIInput::doVerStretch(int index, QPoint pos, QWidget *item  )
{
    // 获取start属性值
    int start_X = item->pos().x();
    int start_Y = item->pos().y();

    // 获取size属性值
    int size_W = item->width();
    int size_H = item->height();

    if ( index == 0 )
    {
        // 设置start和size属性值
        int newsize_w = size_W;
        int newsize_h = size_H+start_Y- static_cast<int>( pos.y() );
        item->setGeometry(start_X,
                          pos.y(),
                          (qAbs(newsize_w)<1)?1:newsize_w,
                          (qAbs(newsize_h)<1)?1:newsize_h);
    }
    else if ( index == 1 )
    {
        // 设置start和size属性值
        int newsize_w = size_W;
        int newsize_h = static_cast<int>(pos.y()) - start_Y;
        item->setGeometry(start_X,
                          start_Y,
                          (qAbs(newsize_w)<1)?1:newsize_w,
                          (qAbs(newsize_h)<1)?1:newsize_h);
    }

    //控件最小size处理
    ItemMiniSize(item);

    //处理组合控件
    DealCombineItem(item);

    item->update();
}


/************************************************************
*函数名称：doFDiagStretch
*函数功能：沿对角线1伸缩操作(左上、右下)
**************************************************************/
void RectEdit_UIInput::doFDiagStretch(int index, QPoint pos, QWidget *item  )
{
    // 获取start属性值
    int start_X = item->pos().x();
    int start_Y = item->pos().y();

    // 获取size属性值
    int size_W = item->width();
    int size_H = item->height();

    if ( index == 0 )
    {
        // 设置start和size属性值
        int newsize_w = size_W+start_X- static_cast<int>(pos.x());
        int newsize_h = size_H+start_Y- static_cast<int>(pos.y());
        item->setGeometry(pos.x(),
                          pos.y(),
                          (qAbs(newsize_w)<1)?1:newsize_w,
                          (qAbs(newsize_h)<1)?1:newsize_h);
    }
    else if ( index == 1 )
    {
        // 设置start和size属性值
        int newsize_w = static_cast<int>(pos.x())-start_X;
        int newsize_h = static_cast<int>(pos.y())-start_Y;
        item->setGeometry(start_X,
                          start_Y,
                          (qAbs(newsize_w)<1)?1:newsize_w,
                          (qAbs(newsize_h)<1)?1:newsize_h);
    }

    ItemMiniSize(item);
    //处理组合控件
    DealCombineItem(item);
    item->update();
}


/************************************************************
*函数名称：doBDiagStretch
*函数功能：沿对角线2伸缩操作(左下、右上)
**************************************************************/
void RectEdit_UIInput::doBDiagStretch(int index, QPoint pos, QWidget *item  )
{
    // 获取start属性值
    int start_X = item->pos().x();
    int start_Y = item->pos().y();

    // 获取size属性值
    int size_W = item->width();
    int size_H = item->height();


    if ( index == 1 )
    {
        //设置start和size属性值
        int newsize_w = size_W+start_X- static_cast<int>(pos.x());
        int newsize_h = static_cast<int>(pos.y())-start_Y;
        item->setGeometry(pos.x(),
                          start_Y,
                          (qAbs(newsize_w)<1)?1:newsize_w,
                          (qAbs(newsize_h)<1)?1:newsize_h);

    }
    else if ( index == 0 )
    {
        //设置start和size属性值
        int newsize_w = static_cast<int>(pos.x())-start_X;
        int newsize_h = size_H+start_Y- static_cast<int>(pos.y());
        item->setGeometry(start_X,
                          pos.y(),
                          (qAbs(newsize_w)<1)?1:newsize_w,
                          (qAbs(newsize_h)<1)?1:newsize_h);

    }
    ItemMiniSize(item);
    //处理组合控件
    DealCombineItem(item);
    item->update();
}


/************************************************************
*函数名称：doItemMove
*函数功能：图形移动操作
**************************************************************/
void RectEdit_UIInput::doItemMove( QPointF dis , QWidget* item )
{
    // 获取start属性值
    int start_X = item->pos().x();
    int start_Y = item->pos().y();

    // 获取size属性值
    int size_W = item->width();
    int size_H = item->height();

    // 重新设置start属性值
    int x = start_X + static_cast<int>( dis.x() );
    int y = start_Y + static_cast<int>( dis.y() );
    if (x < 0)
    {
        x = 0;
    }
    else if (x+size_W > mScreen->rect().right())
    {
        x = mScreen->rect().right() - size_W;
    }

    if (y < 0)
    {
        y = 0;
    }
    else if (y+size_H > mScreen->rect().bottom())
    {
        y = mScreen->rect().bottom() - size_H;
    }
    item->move(x,y);
    item->update();
}

/*virtual*/ void RectEdit_UIInput::setScreen(UWidgetScene *scr)
{
    mScreen = scr;
    m_pressFlag = false;
}

/*virtual*/ bool RectEdit_UIInput::keyPressEvent( QKeyEvent * event )
{
    QList<QWidget *> selectItems =  mScreen->getOptItems();
    foreach (auto item, selectItems)
    {
        if (event->key() == Qt::Key_Up)
        {
            doItemMove(QPoint(0,-5),item);
        }
        else if (event->key() == Qt::Key_Down)
        {
            doItemMove(QPoint(0,5),item);
        }
        else if (event->key() == Qt::Key_Left)
        {
            doItemMove(QPoint(-5,0),item);
        }
        else if (event->key() == Qt::Key_Right)
        {
            doItemMove(QPoint(5,0),item);
        }
    }
    return false;
}

void RectEdit_UIInput::dealMouseType(const QPointF &curPos,QRectF normalrectF)
{
    QRectF lefttopRectF(normalrectF.topLeft().x(),normalrectF.topLeft().y(),Mouse_Type_Rect,Mouse_Type_Rect);
    QRectF rightTopRectF(normalrectF.topRight().x()-Mouse_Type_Rect,normalrectF.topRight().y(),Mouse_Type_Rect,Mouse_Type_Rect);
    QRectF bottomLeftRectF(normalrectF.bottomLeft().x(),normalrectF.bottomLeft().y()-Mouse_Type_Rect,Mouse_Type_Rect,Mouse_Type_Rect);
    QRectF bottomRightRectF(normalrectF.bottomRight().x()-Mouse_Type_Rect,normalrectF.bottomRight().y()-Mouse_Type_Rect,Mouse_Type_Rect,Mouse_Type_Rect);

    if( lefttopRectF.contains(curPos))
    {// 沿对角0
        mScreen->setCursor(Qt::SizeFDiagCursor );
        m_mouseIndex = Upper_Left;
    }
    else if(bottomRightRectF.contains(curPos) )
    {
         mScreen->setCursor(Qt::SizeFDiagCursor );
         m_mouseIndex = Lower_Right;
    }
    else if( rightTopRectF.contains(curPos)  )
    {// 沿对角1
        mScreen->setCursor(Qt::SizeBDiagCursor );
        m_mouseIndex = Upper_Right;
    }
    else if(bottomLeftRectF.contains(curPos))
    {
        mScreen->setCursor(Qt::SizeBDiagCursor );
        m_mouseIndex = Lower_Left;
    }
    else if( ( lefttopRectF.x()+Mouse_Type_Rect > curPos.x()  && lefttopRectF.x()-Mouse_Type_Rect < curPos.x() ) )
    {//水平调整
        mScreen->setCursor(Qt::SizeHorCursor );
        m_mouseIndex =  Horizontal_Left;
    }
    else if(rightTopRectF.x()+Mouse_Type_Rect > curPos.x()  && rightTopRectF.x()-Mouse_Type_Rect < curPos.x())
    {
        mScreen->setCursor(Qt::SizeHorCursor );
        m_mouseIndex =  Horizontal_Right;
    }
    else if( ( lefttopRectF.y()+Mouse_Type_Rect > curPos.y()  && lefttopRectF.y()-Mouse_Type_Rect < curPos.y() )  )
    {// 垂直调整
        mScreen->setCursor(Qt::SizeVerCursor );
        m_mouseIndex =  Vertical_Up;
    }
    else if(bottomRightRectF.y()+Mouse_Type_Rect > curPos.y()  && bottomRightRectF.y()-Mouse_Type_Rect < curPos.y())
    {
        mScreen->setCursor(Qt::SizeVerCursor );
         m_mouseIndex =  Vertical_Down;
    }
    else
    {
        m_mouseIndex = mouse_nor;
        mScreen->setCursor(Qt::SizeAllCursor);
    }
}

void RectEdit_UIInput::DealCombineItem(QWidget *item)
{
    if(COMBINE_ITEM == item->property("z_class_name").toString())
    {
        double zoomW = static_cast<double>( item->width()/static_cast<double>(item->property(COMBINE_ITEM_PARENT_W).toInt()) );
        double zoomH =  static_cast<double>(item->height()/static_cast<double>(item->property(COMBINE_ITEM_PARENT_H).toInt()) );
        QObjectList objlist = item->children();
        QString className;
        foreach (auto itemobj, objlist)
        {//
           QWidget * itemTmp = qobject_cast<QWidget*>(itemobj);
           if(itemTmp)
           {
               className = itemTmp->property("z_class_name").toString();
               itemTmp->setGeometry(static_cast<int>( itemTmp->property(COMBINE_ITEM_X).toInt()*zoomW ),
                                    static_cast<int>( itemTmp->property(COMBINE_ITEM_Y).toInt()*zoomH ),
                                    static_cast<int>( itemTmp->property(COMBINE_ITEM_W).toInt()*zoomW ),
                                    static_cast<int>( itemTmp->property(COMBINE_ITEM_H).toInt()*zoomH) );

               //处理组合控件
               DealCombineItem(itemTmp);
           }
        }
    }
}

void RectEdit_UIInput::ItemMiniSize(QWidget * item)
{
    if(item->width()<= 20 && item->height()<= 20)
    {
       item->setGeometry(item->pos().x(),item->pos().y(),20,20);
    }
    if(item->width()<= 5)
    {
        item->setGeometry(item->pos().x(),item->pos().y(),
                          5,item->height());
    }
    if(item->height()<= 5)
    {
        item->setProperty("z_h",5);
        item->setGeometry(item->pos().x(),item->pos().y(),
                          item->width(),5);
    }
}

void RectEdit_UIInput::DealMouseType(const QPoint &pos)
{
    // 获取鼠标所在位置的item
    QWidget* item = mScreen->itemAt(pos);
    if( !item )
    {
        // 鼠标移动的位置没有item或者不是选中的item
        mScreen->setCursorType( Qt::ArrowCursor );
    }
    else
    {
        //如果控件是被选中的控件
        if(mScreen->isSelected(item))
        {
            // 鼠标正好在选中的item上移动
            mScreen->setCursorType( Qt::SizeAllCursor );
            //获取polygonF,设置鼠标样式等等
            dealMouseType(pos,QRectF(item->x(),item->y(),item->rect().width(),item->rect().height()));
        }
        else
        {
            mScreen->setCursorType( Qt::ArrowCursor );
        }
    }
}
