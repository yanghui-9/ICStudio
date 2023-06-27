#ifndef UTOOLBAR_H
#define UTOOLBAR_H

#include <QObject>

class MainWindow;
class QMenu;
class QToolBar;
class QAction;
class UToolbar : public QObject
{
    Q_OBJECT
public:
    enum VIEW_OPT
    {
        //撤销
        VIEW_OPT_UNDO = 0,
        VIEW_OPT_REDO ,
        //复制
        VIEW_OPT_COPY,
        VIEW_OPT_PASTE,
        VIEW_OPT_DELETE,
        VIEW_OPT_SEATCH,
        //组合
        VIEW_OPT_COMBINE,
        VIEW_OPT_UNCOMBINE,
        //锁定解锁
        VIEW_OPT_LOCK,
        VIEW_OPT_UNLOCK,
        //全选反选
        VIEW_OPT_SELECT_ALL,
        VIEW_OPT_REVERSE_SELECT,

        //移动
        VIEW_OPT_MOVE_UP,
        VIEW_OPT_MOVE_DOWN,
        VIEW_OPT_MOVE_LEFT,
        VIEW_OPT_MOVE_RIGHT,

        //对齐
        VIEW_OPT_ALIGN_UP,
        VIEW_OPT_ALIGN_DOWN,
        VIEW_OPT_ALIGN_LEFT,
        VIEW_OPT_ALIGN_RIGHT,
        //垂直和水平居中
        VIEW_OPT_CENTER_VERTICALLY,
        VIEW_OPT_CENTER_HORIZONTALLY,
        //垂直和水平等距
        VIEW_OPT_HORIZONTAL_ISOMETRIC,
        VIEW_OPT_VERTICAL_ISOMETRIC,
        //等宽等高
        VIEW_OPT_EQUAL_WIDTH,
        VIEW_OPT_EQUAL_HEIGHT,
        VIEW_OPT_EQUAL_WIDTH_HEIGHT,

        //层次移动
        VIEW_OPT_MOVE_TO_TOP,
        VIEW_OPT_MOVE_TO_FRONT,
        VIEW_OPT_MOVE_TO_BOTTOM,
        VIEW_OPT_MOVE_TO_BACK,

        //保存为自定义控件
        VIEW_OPT_SAVE_CUSTOM,

        //画面属性
        VIEW_OPT_PROPERTY
    };
    enum VIEW_OPT_TYPE
    {
        VIEW_TREE_ITEM_ROOT_SCENE = 1,
        VIEW_TREE_ITEM_ROOT_WINDOW,
        VIEW_TREE_ITEM_ROOT_TEMPLATE,
    };
    explicit UToolbar(QObject *parent = nullptr);
    void Init();

    //显示隐藏
    void SetToolHide(bool flag);

    //显示菜单
    void ShowMenu();

    //获取显示控制action
    QList<QAction *> GetAclist();


protected:
    void InitActionFromName(QString name, VIEW_OPT vOpt, bool bSeparator = false);
    void InitAction();
    void InitShortcutKey();

public slots:
    //组态编辑槽函数
    void ItemEditSlot();


private:
    MainWindow * m_mainW;
    QList<QObject *> m_tools;
    QList<QString>   m_toolNames;
    QToolBar * m_toolBar;
    QToolBar * m_itemsToolBar;

    //工具菜单
    QMenu *m_SceneMenu = nullptr;
    QMenu * m_SceneMenuAlignMent;
    QMenu * m_SceneMenuLevel;
    QMenu * m_SceneMenuScene;
};

#endif // UTOOLBAR_H
