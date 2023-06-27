#include "UToolbar.h"
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QCursor>
#include "MainWindow.h"
#include "Itemsfactory.h"

UToolbar::UToolbar(QObject *parent) : QObject(parent)
{
    m_mainW = qobject_cast<MainWindow*>(parent);
}

void UToolbar::Init()
{
    m_toolBar = new QToolBar(QString::fromLocal8Bit("控件编辑操作"),m_mainW);

    //画面组态Action
    InitAction();
    for (int i = 0; i < m_tools.size(); ++i)
    {
        m_tools.at(i)->metaObject()->className();
        if(0 == strcmp(m_tools.at(i)->metaObject()->className(),"QAction"))
        {//qaction
            m_toolBar->addAction(qobject_cast<QAction*>(m_tools.at(i)));
        }
        else
        {//qwidget
            m_toolBar->addWidget(qobject_cast<QWidget*>(m_tools.at(i)));
        }
        if(m_tools.at(i)->property("addSeparator").toBool())
        {
           m_toolBar->addSeparator();
        }
    }
    m_mainW->addToolBar(m_toolBar);

    //控件列表Action
    m_mainW->addToolBarBreak();
    m_itemsToolBar = new QToolBar(QString::fromLocal8Bit("控件"),m_mainW);
    QList<QString> itemsName;
    QList<QAction*> itemsAc;
    itemsFactory::GetInstance()->getItemProList(itemsName,itemsAc);
    m_itemsToolBar->addActions(itemsAc);
    m_mainW->addToolBar(m_itemsToolBar);
}

void UToolbar::SetToolHide(bool flag)
{
    m_toolBar->setVisible(flag);
    m_itemsToolBar->setVisible(flag);
}

void UToolbar::ShowMenu()
{
    if(m_SceneMenu)
    {
        m_SceneMenu->exec(QCursor::pos());
    }
}

QList<QAction *> UToolbar::GetAclist()
{
   QList<QAction*> aclist;
   aclist.append(m_toolBar->toggleViewAction());
   aclist.append(m_itemsToolBar->toggleViewAction());
   return aclist;
}

void UToolbar::InitActionFromName(QString name,VIEW_OPT vOpt,bool bSeparator)
{
    QAction *tmpAct = new QAction(m_mainW);
    tmpAct->setIcon(QIcon(QString(":/image/%1.png").arg(name)));
    tmpAct->setText(name);
    if(bSeparator)
    {
        tmpAct->setProperty("addSeparator",true);
    }
    tmpAct->setProperty("VIEW_OPT",vOpt);
    connect(tmpAct,SIGNAL(triggered()),this,SLOT(ItemEditSlot()));
    m_tools.push_back(tmpAct);
    m_toolNames.push_back(name);
}

void UToolbar::InitAction()
{
    if(0 ==  m_tools.size())
    {
        InitActionFromName(QObject::tr("undo"),VIEW_OPT_UNDO);
        qobject_cast<QAction*>(m_tools.back())->setShortcut(QKeySequence::Undo);
        InitActionFromName(QObject::tr("redo"),VIEW_OPT_REDO,true);
        qobject_cast<QAction*>(m_tools.back())->setShortcut(QKeySequence::Redo);

        InitActionFromName(QObject::tr("copy"),VIEW_OPT_COPY);
        qobject_cast<QAction*>(m_tools.back())->setShortcut(QKeySequence::Copy);
        InitActionFromName(QObject::tr("paste"),VIEW_OPT_PASTE);
        qobject_cast<QAction*>(m_tools.back())->setShortcut(QKeySequence::Paste);
        InitActionFromName(QObject::tr("delete"),VIEW_OPT_DELETE,true);
        qobject_cast<QAction*>(m_tools.back())->setShortcut(QKeySequence::Delete);
        InitActionFromName(QObject::tr("search"),VIEW_OPT_SEATCH,true);
        qobject_cast<QAction*>(m_tools.back())->setShortcut(QKeySequence::Find);
        InitActionFromName(QObject::tr("group"),VIEW_OPT_COMBINE);
        InitActionFromName(QObject::tr("ungroup"),VIEW_OPT_UNCOMBINE,true);
        InitActionFromName(QObject::tr("lock"),VIEW_OPT_LOCK);
        InitActionFromName(QObject::tr("unlock"),VIEW_OPT_UNLOCK,true);
        InitActionFromName(QObject::tr("select all"),VIEW_OPT_SELECT_ALL);
        qobject_cast<QAction*>(m_tools.back())->setShortcut(QKeySequence::SelectAll);
        InitActionFromName(QObject::tr("reverse select"),VIEW_OPT_REVERSE_SELECT,true);

        InitActionFromName(QObject::tr("move up"),VIEW_OPT_MOVE_UP);
        InitActionFromName(QObject::tr("move down"),VIEW_OPT_MOVE_DOWN);
        InitActionFromName(QObject::tr("move left"),VIEW_OPT_MOVE_LEFT);
        InitActionFromName(QObject::tr("move right"),VIEW_OPT_MOVE_RIGHT,true);

        InitActionFromName(QObject::tr("top"),VIEW_OPT_ALIGN_UP);
        InitActionFromName(QObject::tr("bottom"),VIEW_OPT_ALIGN_DOWN);
        InitActionFromName(QObject::tr("left"),VIEW_OPT_ALIGN_LEFT);
        InitActionFromName(QObject::tr("right"),VIEW_OPT_ALIGN_RIGHT);
        InitActionFromName(QObject::tr("vertical center"),VIEW_OPT_CENTER_VERTICALLY);
        InitActionFromName(QObject::tr("horizontal center"),VIEW_OPT_CENTER_HORIZONTALLY,true);
        InitActionFromName(QObject::tr("vertical equidistant"),VIEW_OPT_VERTICAL_ISOMETRIC);
        InitActionFromName(QObject::tr("horizontal equidistant"),VIEW_OPT_HORIZONTAL_ISOMETRIC,true);
        InitActionFromName(QObject::tr("make same width"),VIEW_OPT_EQUAL_WIDTH);
        InitActionFromName(QObject::tr("make same height"),VIEW_OPT_EQUAL_HEIGHT);
        InitActionFromName(QObject::tr("make same size"),VIEW_OPT_EQUAL_WIDTH_HEIGHT,true);

        InitActionFromName(QObject::tr("move to top"),VIEW_OPT_MOVE_TO_TOP);
        InitActionFromName(QObject::tr("move to front"),VIEW_OPT_MOVE_TO_FRONT);
        InitActionFromName(QObject::tr("move to bottom"),VIEW_OPT_MOVE_TO_BOTTOM);
        InitActionFromName(QObject::tr("move to back"),VIEW_OPT_MOVE_TO_BACK,true);

        InitActionFromName(QObject::tr("save custom item"),VIEW_OPT_SAVE_CUSTOM,true);

        InitActionFromName(QObject::tr("property"),VIEW_OPT_PROPERTY);


        //初始画面右键菜单
        m_SceneMenu = new QMenu(m_mainW->m_View);
        for (int i = 0; i < VIEW_OPT_REVERSE_SELECT; ++i) {
            m_SceneMenu->addAction(qobject_cast<QAction*>(m_tools.at(i)));
        }

        m_SceneMenuAlignMent = new QMenu(m_mainW);
        m_SceneMenuAlignMent->setTitle(QObject::tr("alignment"));
        for (int i = VIEW_OPT_ALIGN_UP; i < VIEW_OPT_EQUAL_WIDTH_HEIGHT-VIEW_OPT_ALIGN_UP+1; ++i) {
            m_SceneMenuAlignMent->addAction(qobject_cast<QAction*>(m_tools.at(i)));
        }
        m_SceneMenu->addMenu(m_SceneMenuAlignMent);

        m_SceneMenuLevel = new QMenu(m_mainW);
        m_SceneMenuLevel->setTitle(QObject::tr("level"));
        for (int i = VIEW_OPT_MOVE_TO_TOP; i < VIEW_OPT_MOVE_TO_BACK-VIEW_OPT_MOVE_TO_TOP+1; ++i) {
            m_SceneMenuLevel->addAction(qobject_cast<QAction*>(m_tools.at(i)));
        }
        m_SceneMenu->addMenu(m_SceneMenuLevel);

        m_SceneMenu->addAction(qobject_cast<QAction*>(m_tools.at(VIEW_OPT_SAVE_CUSTOM)));

        m_SceneMenu->addAction(qobject_cast<QAction*>(m_tools.at(VIEW_OPT_PROPERTY)));

        //初始化快捷键
        InitShortcutKey();
    }
}

void UToolbar::InitShortcutKey()
{
    //初始化快捷键
    qobject_cast<QAction*>(m_tools.at(VIEW_OPT_COPY))->setShortcut(QKeySequence("Ctrl+C"));
    qobject_cast<QAction*>(m_tools.at(VIEW_OPT_PASTE))->setShortcut(QKeySequence("Ctrl+V"));
    qobject_cast<QAction*>(m_tools.at(VIEW_OPT_DELETE))->setShortcut(QKeySequence("Del"));

    qobject_cast<QAction*>(m_tools.at(VIEW_OPT_UNDO))->setShortcut(QKeySequence("Ctrl+Z"));
    qobject_cast<QAction*>(m_tools.at(VIEW_OPT_REDO))->setShortcut(QKeySequence("Ctrl+Y"));
    qobject_cast<QAction*>(m_tools.at(VIEW_OPT_SELECT_ALL))->setShortcut(QKeySequence("Ctrl+A"));
}

void UToolbar::ItemEditSlot()
{
    if(!m_mainW)
    {
        return;
    }
    QAction * act = qobject_cast<QAction*>(sender());
    if(act)
    {
        int index =  act->property("VIEW_OPT").toInt();
        switch (index) {
        case VIEW_OPT_UNDO:
        {
            m_mainW->m_View->curScene()->ItemUndo();
        }
            break;
        case VIEW_OPT_REDO:
        {
            m_mainW->m_View->curScene()->ItemRedo();
        }
            break;
        case VIEW_OPT_COPY:
        {
            m_mainW->m_View->curScene()->ItemCopy();
        }
            break;
        case VIEW_OPT_PASTE:
        {
            m_mainW->m_View->curScene()->ItemPaste();
        }
            break;
        case VIEW_OPT_DELETE:
        {
            m_mainW->m_View->curScene()->ItemDelete();
        }
            break;
        case VIEW_OPT_COMBINE:
        {
            m_mainW->m_View->curScene()->ItemCombine();
        }
            break;
        case VIEW_OPT_UNCOMBINE:
        {
            m_mainW->m_View->curScene()->ItemUnCombine();
        }
            break;
        case VIEW_OPT_LOCK:
        {
            m_mainW->m_View->curScene()->ItemLock();
        }
            break;
        case VIEW_OPT_UNLOCK:
        {
            m_mainW->m_View->curScene()->ItemUnLock();
        }
            break;
        case VIEW_OPT_SELECT_ALL:
        {
            m_mainW->m_View->curScene()->setSelectitemsAtRect(QRect(m_mainW->m_View->curScene()->pos(),
                                                                    m_mainW->m_View->curScene()->size()));
        }
            break;
        case VIEW_OPT_REVERSE_SELECT:
        {
            m_mainW->m_View->curScene()->setSelectitemsAtRect(QRect(m_mainW->m_View->curScene()->pos(),
                                                                    m_mainW->m_View->curScene()->size()),false);
        }
            break;
        case VIEW_OPT_MOVE_UP://移动
        {
            m_mainW->m_View->curScene()->ItemMvoe(UWidgetScene::move_up);
        }
            break;
        case VIEW_OPT_MOVE_DOWN:
        {
            m_mainW->m_View->curScene()->ItemMvoe(UWidgetScene::move_down);
        }
            break;
        case VIEW_OPT_MOVE_LEFT:
        {
            m_mainW->m_View->curScene()->ItemMvoe(UWidgetScene::move_left);
        }
            break;
        case VIEW_OPT_MOVE_RIGHT:
        {
            m_mainW->m_View->curScene()->ItemMvoe(UWidgetScene::move_right);
        }
            break;
        case VIEW_OPT_ALIGN_UP://上下左右对齐
        {
            m_mainW->m_View->curScene()->ItemAlignUp();
        }
            break;
        case VIEW_OPT_ALIGN_DOWN:
        {
            m_mainW->m_View->curScene()->ItemAlignDown();
        }
            break;
        case VIEW_OPT_ALIGN_LEFT:
        {
            m_mainW->m_View->curScene()->ItemAlignLeft();
        }
            break;
        case VIEW_OPT_ALIGN_RIGHT:
        {
            m_mainW->m_View->curScene()->ItemAlignRight();
        }
            break;
        case VIEW_OPT_CENTER_VERTICALLY://垂直水平居中对齐
        {
            m_mainW->m_View->curScene()->ItemAlignCenterVer();
        }
            break;
        case VIEW_OPT_CENTER_HORIZONTALLY:
        {
            m_mainW->m_View->curScene()->ItemAlignCenterHor();
        }
            break;
        case VIEW_OPT_HORIZONTAL_ISOMETRIC://垂直水平等距
        {
            m_mainW->m_View->curScene()->ItemIsometricHor();
        }
            break;
        case VIEW_OPT_VERTICAL_ISOMETRIC:
        {
            m_mainW->m_View->curScene()->ItemIsometricVer();
        }
            break;
        case VIEW_OPT_EQUAL_WIDTH://等宽等高
        {
            m_mainW->m_View->curScene()->ItemEqualWidth();
        }
            break;
        case VIEW_OPT_EQUAL_HEIGHT:
        {
            m_mainW->m_View->curScene()->ItemEqualHeight();
        }
            break;
        case VIEW_OPT_EQUAL_WIDTH_HEIGHT:
        {
            m_mainW->m_View->curScene()->ItemEqualWH();
        }
            break;
        case VIEW_OPT_MOVE_TO_TOP://层次
        {
            m_mainW->m_View->curScene()->ItemRaise();
        }
            break;
        case VIEW_OPT_MOVE_TO_FRONT:
        {
            m_mainW->m_View->curScene()->ItemZvaluePlus();
        }
            break;
        case VIEW_OPT_MOVE_TO_BOTTOM:
        {
            m_mainW->m_View->curScene()->ItemLower();
        }
            break;
        case VIEW_OPT_MOVE_TO_BACK:
        {
            m_mainW->m_View->curScene()->ItemZvalueMinus();
        }
            break;
        case VIEW_OPT_SAVE_CUSTOM://保存自定义控件
        {
            m_mainW->m_View->curScene()->ItemSaveCustomItem();
        }
            break;
        case VIEW_OPT_PROPERTY://画面属性
        {
            m_mainW->m_View->curScene()->SceneProEdit();
        }
            break;
        }
    }
}

