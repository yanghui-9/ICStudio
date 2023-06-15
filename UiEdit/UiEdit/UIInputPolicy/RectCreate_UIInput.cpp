#include "RectCreate_UIInput.h"
#include "qmath.h"
#include <QDebug>
#include "Itemsfactory.h"
#include "RectEdit_UIInput.h"


RectCreate_UIInput::RectCreate_UIInput()
{
}

/*virtual*/ bool RectCreate_UIInput::mousePress( QMouseEvent  * evt )
{
    if(!mScreen)
    {
        return true;
    }

    if( evt->button() == Qt::RightButton )
    {
        return true;
    }

    //鼠标样式设置
    mScreen->setCursorType( Qt::CrossCursor );

    //创建控件
    QString name = mScreen->getStrNewType();
    item = itemsFactory::GetInstance()->createItemFromName(name,mScreen);
    if(!item)
    {//创建失败
        return true;
    }
    itemsFactory::GetInstance()->createItemObjFromName(name,item);
    item->setProperty("z_class_name",name);

    // scene取消选择
    mScreen->cancelSelect();

    if( item)
    {
        //添加到画面
        mScreen->addItem(item,evt->pos(),QSize(10,10));

        // 将item设置成选中状态
        mScreen->setSelected(item);

        // 更新item
        item->update();
    }
    return true;
}

/*virtual*/ bool RectCreate_UIInput::mouseMove(QMouseEvent *evt )
{
    if(!mScreen)
        return false;

    mScreen->setCursorType( Qt::CrossCursor );

    if ( item )
    {
        item->setGeometry(item->pos().x(),
                          item->pos().y(),
                          evt->pos().x()-item->pos().x(),
                          evt->pos().y()-item->pos().y());

        mScreen->setSelected(item);
        item->update();
        RectEdit_UIInput::DealCombineItem(item);
    }

    return false;
}

/*virtual*/ bool RectCreate_UIInput::mouseRelease(QMouseEvent * /*evt*/ )
{
    if(!mScreen)
        return false;
    mScreen->setCursorType( Qt::ArrowCursor );
    if ( item )
    {
        // 创建item，入栈操作
        //GraphicsScene * scene = (GraphicsScene *)(mScreen->getScene());
        //SceneCreateItemUndo * addCommand = new SceneCreateItemUndo(item, mScreen);
        //mScreen->getUndoStack()->push(addCommand);

        // 发送工程修改信号
        mScreen->SetChangeFlag();

        mScreen->setSelected(item);
        item->update();
        item = nullptr;

        mScreen->SetChangeFlag();
    }
    mScreen->setSelectMode();
    return true;
}

/*virtual*/ void RectCreate_UIInput::setScreen(UWidgetScene  *scr)
{
    mScreen = scr;
}

/*virtual*/ bool RectCreate_UIInput::keyPressEvent( QKeyEvent * /*event*/ )
{
    return false;
}
