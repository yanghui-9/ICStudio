#include <QtCore/qmath.h>
#include <QPainter>
#include <QDebug>
#include "Select_UIInput.h"

Select_UIInput::Select_UIInput()
{
    m_pressFlag = false;
    m_mulselectFlag = false;
    m_pflag = false;
    m_selectRectItem = nullptr;
}

/*virtual*/ bool Select_UIInput::mousePress(QMouseEvent *evt )
{
    m_pressPos = evt->pos();
    m_pressFlag = true;

    if( !mScreen )
        return false;

    QWidget* item = mScreen->itemAt( m_pressPos);
    if( item )
    {// 按下的位置正好有item
        //默认去除选择框
        item->setFocus();
        m_pressFlag = false;
        //如果选择到控件直接进入编辑模式
        mScreen->setEditMode();
        mScreen->setCursorType( Qt::SizeAllCursor );
        return false;
    }
    else
    {
        mScreen->setCursorType( Qt::ArrowCursor );
        //选择框
        if(m_selectRectItem)
        {
            m_selectRect.setRect(0,0,0,0);
            m_selectRectItem->raise();
            m_selectRectItem->show();
            m_selectRectItem->setGeometry(m_pressPos.x(),m_pressPos.y(),1,1);
        }
    }
    return true;
}


/*virtual*/ bool Select_UIInput::mouseMove(QMouseEvent *evt )
{
    if( !mScreen )
        return false;
    //选择框范围
    m_selectRect = QRect( m_pressPos,evt->pos());

    //是否鼠标按下
    if( !m_pressFlag )
    {
        QWidget * item = mScreen->itemAt( evt->pos() );

        if( item )
        {// 鼠标在非选中的item上移动
            mScreen->setCursorType( Qt::SizeAllCursor );
        }
        else
        {// 鼠标在空白处移动
            mScreen->setCursorType( Qt::ArrowCursor );
        }
    }
    else
    {
        //处理选择框
        if(m_selectRectItem)
        {
            m_selectRectItem->setGeometry(m_selectRect);
        }
    }
    return false;
}

/*virtual*/ bool Select_UIInput::mouseRelease(QMouseEvent *evt)
{
    if( !mScreen )
        return false;

    //选择框
    if(m_selectRectItem)
    {
        //选择框去除
        m_selectRectItem->hide();
        //先取消所有控件选中
        mScreen->cancelSelect();
        //设置选择区域内控件全部选中
        QList<QWidget*> selectItems = mScreen->setSelectitemsAtRect(m_selectRect);
        //如果选择到控件直接进入编辑模式
        if(0 < selectItems.size())
        {
            mScreen->setEditMode();
            mScreen->setCursorType( Qt::SizeAllCursor );
        }
    }

    m_selectRect = QRect( m_pressPos,evt->pos());
    m_pressFlag = false;
    m_mulselectFlag = false;

    //遍历画面中所有控件是否和选择区域交叉
    if( mScreen->selectedItems().count() != 0 )
    {
        mScreen->setEditMode();
        return true;
    }
    return false;
}

/*virtual*/ void Select_UIInput::setScreen(UWidgetScene *scr)
{
    mScreen = scr;
    InitSelectItem();
}

/*virtual*/ bool Select_UIInput::keyPressEvent( QKeyEvent * /*event*/ )
{
    return false;
}

void Select_UIInput::InitSelectItem()
{
    if(!m_selectRectItem)
    {
        m_selectRectItem = new QWidget(mScreen);
        //设置背景色
        QPalette palette = m_selectRectItem->palette();
        palette.setColor(QPalette::Window,QColor(180,180,180,150));
        m_selectRectItem->setAutoFillBackground(true);
        m_selectRectItem->setPalette(palette);
    }
    else
    {
        m_selectRectItem->setParent(mScreen);
    }
    m_selectRectItem->hide();
}
