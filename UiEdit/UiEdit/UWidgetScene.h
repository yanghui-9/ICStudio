#ifndef UWidgetScene_H
#define UWidgetScene_H

#include <QWidget>
#include <QtCore/QMap>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QLabel>
#include <QMutex>
#include <QUndoStack>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QLinkedList>
#include "../Items/interface_item.h"
#include "UIInputPolicy/IUIInputPolicy.h"

#define COMBINE_ITEM "combine"
#define COMBINE_ITEM_PATH "./CustomComponents"
#define COMBINE_ITEM_PARENT_W "combine_parent_w"
#define COMBINE_ITEM_PARENT_H "combine_parent_h"
#define COMBINE_ITEM_H "combine_h"
#define COMBINE_ITEM_W "combine_w"
#define COMBINE_ITEM_H "combine_h"
#define COMBINE_ITEM_X "combine_X"
#define COMBINE_ITEM_Y "combine_Y"
#define UNDO_LIMIT 20

class UTabWidget;
class MainStatusBar;
class UWidgetScene : public QWidget
{
    Q_OBJECT
    friend class UTabWidget;
public:
    enum Move_Type
    {
        move_up = 0,
        move_down,
        move_left,
        move_right
    };
    explicit UWidgetScene(int type, QString name, UTabWidget * view);
    virtual ~UWidgetScene();

    //加载画面控件
    void loadSceneItem(const QString &sdata);

    //保存画面配置
    void saveSceneItem(QString &sdata);

    ////处理画面打开关闭
    void dealScenceClose();
    void dealScenceOpen();

    ///刷新所有
    void reflashAllItem();

    //删除指定控件
    void removeItemObject(QList<QWidget*> items, bool isUndo = false);

    ///基本操作
    int getType(){ return m_sceneObj["z_type"].toInt(0); }
    void setType(int type){ m_sceneObj["z_type"] = type; }
    QString getSceneName(){return m_name;}
    void setSceneName(QString name){m_name = name;}
    QRect getSceneRect();

    //编辑action
    void ItemUndo();//undo
    void ItemRedo();//redo
    void ItemCopy();//复制
    void ItemPaste();//粘贴
    void ItemDelete();//删除
    void ItemMvoe(Move_Type type);//移动
    void ItemLower();//层次最底层
    void ItemRaise();//层次顶层
    void ItemZvalueMinus();//层次减一
    void ItemZvaluePlus();//层次加一
    void ItemLock();//锁定
    void ItemUnLock();//解锁
    void ItemCombine();//组合
    void ItemUnCombine();//取消组合
    void ItemAlignUp();//向上对齐
    void ItemAlignDown();//向下对齐
    void ItemAlignLeft();//向左对齐
    void ItemAlignRight();//向右对齐
    void ItemAlignCenterVer();//垂直居中对齐
    void ItemAlignCenterHor();//水平居中对齐
    void ItemIsometricVer();//垂平等距
    void ItemIsometricHor();//水平等距
    void ItemEqualWidth();//等宽
    void ItemEqualHeight();//等高
    void ItemEqualWH();//等宽高
    void ItemSaveCustomItem();//保存自定义控件
    void SceneProEdit();//属性


    //编辑操作
    bool addItem(QWidget * child, QPoint pos, QSize size);
    bool addItem(QWidget * child);
    QWidget * cloneOfItem(QWidget * item, QWidget *parent  = nullptr);
    QWidget * itemAt(const QPoint &pos);//获取坐标上最上层控件
    QList<QWidget *> setSelectitemsAtRect(const QRect &rect,bool flag = true);//获取范围内所有控件选中
    QList<QWidget *> items();
    void setSelected(QWidget * item,bool flag = true);
    bool isSelected(QWidget * item);
    QList<QWidget *> selectedItems();
    QList<QWidget *> getOptItems();
    void cancelSelect();

    //设置编辑模式
    void setCreateMode(const QString& itemName);
    void setSelectMode(void);
    void setEditMode(void);
    void setUIMode(IUIInputPolicy * policy);
    void setCursorType( Qt::CursorShape curShape );

    //获取鼠标样式
    Qt::CursorShape getCursorShape();

    //设置变化标识
    void SetChangeFlag();

    //undo/redo操作
    QUndoStack * getUndoStack(){return m_sceneUndoStack;}

    //获取创建
    QString getStrNewType(){return m_creatPolicyItemName;}

    //状态栏
    void setMainStatusBar(MainStatusBar *statusBar){m_statusBar = statusBar;}

    //获取图库
    interface_gallery* GetGallery();

    //通过json加载控件
    void loadItemsFromJson(QJsonArray &itemArray, QList<QWidget *> &itemList, QWidget *parent = nullptr);

    //显示窗口
    void ShowWindow(QWidget * window, QRect rect);

protected:
    //采集地址信息（切换画面时）
    void AssociateAddressAndItem();

    //鼠标事件
    void mousePressEvent(QMouseEvent  *evt);
    void mouseMoveEvent(QMouseEvent  *evt);
    void mouseReleaseEvent(QMouseEvent  *evt);

    //键盘
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    //编译
    QStringList compile();
    //处理编译错误
    void dealCompileError(QStringList &errInfoList);

    //通过json加载控件
    void saveItemsFromJson(QJsonArray &itemArray, QList<QWidget *> &itemList);

    //处理控件粘贴
    void dealItemPaste(QList<QWidget *> items, QList<QWidget *> itemsNewList, QWidget * parent = nullptr);

    //初始化控件
    void initItem(QWidget *newitem, const QString &sClassName,int x,int y,int w,int h);

    //处理画面所有控件通过flag
    void DealAllItem(int flag);

    //通过配置初始化画面信息
    void initScenePropertyFromObj();

    //初始化控件
    //void setEditOrRunMode(QWidget *item);

    //获取控件属性
    void GetItemPro(QWidget *item,QJsonObject &obj);

protected slots:
    //画面循环脚本定时器
    void sceneCycScriptSlot();

private:
    //控件列表
    QList<QWidget *> m_itemList;
    QMutex m_loadItemMutex;
    //地址变量列表-控件
    QList <Protocol::AddrInfoForRW> m_addrsInfo;
    //undo
    QUndoStack * m_sceneUndoStack = nullptr;
    //当前编辑模式
    IUIInputPolicy * m_currentPolicy = nullptr;
    //当前创建控件类名称
    QString m_creatPolicyItemName;
    //视图窗口
    UTabWidget * m_view = nullptr;
    //状态栏
    MainStatusBar * m_statusBar = nullptr;
    //画面配置数据（json）
    QString m_sceneObject;
    QJsonObject m_sceneObj;
    //画面名称
    QString m_name;
    //模式
    bool m_modeFlag = false;//true-编辑
    //画面循环脚本
    QTimer m_cycScript;
};

#endif // UGRAPHICSSCENEHMI_H
