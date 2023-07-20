#ifndef UTabWidget_H
#define UTabWidget_H

#include <QList>
#include <QMap>
#include <QTabWidget>
#include "UWidgetScene.h"
#include "UIInputPolicy/RectCreate_UIInput.h"
#include "UIInputPolicy/Select_UIInput.h"
#include "UIInputPolicy/RectEdit_UIInput.h"

class MainWindow;
class UTabWidget : public QTabWidget
{
    Q_OBJECT
    friend class UWidgetScene;
    friend class MainWindow;
public:
    enum Scene_Type
    {
        Scene = 0,
        Window,
    };
    explicit UTabWidget(QWidget *parent = nullptr);
    virtual ~UTabWidget();

    //画面操作
    int setCurScene(const QString &name);
    void setCurScene(UWidgetScene *scene);
    int addScene(QString name, UWidgetScene *scene, bool isScene = false);
    int deleteScene(const QString &name);
    int deleteScene(UWidgetScene *scene);
    UWidgetScene* getSceneFromName(const QString &name);

    //编译
    QStringList compile();

    //处理编译错误
    void dealCompileError(QStringList &errInfoList);

    //设置变化标识
    void SetChangeFlag(QWidget * w);

    //获取当前画面
    UWidgetScene *curScene();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    int initScene(const QString &name);//初始化所有画面控件加载，后台线程调用

public slots:
    void customContextMenuRequestedSlot(QPoint pos);//根据坐标显示右键菜单


private:
    //记录当前画面
    QString m_curSceneID = "";
    //记录前一个窗口
    QString m_windowID = "";
    QMap<QString,UWidgetScene*> m_sceneList;
    //主窗口
    MainWindow * m_mainW;

    //编辑模式
    IUIInputPolicy * RectCreateUIInput = nullptr;
    IUIInputPolicy * RectEditUIInput = nullptr;
    IUIInputPolicy * selectUIInput = nullptr;

    //待复制控件
    QList<QWidget *> m_copyList;

    //当前画面列表，开机时初始化
    QStringList m_sSceneList;

    //运行时弹出窗口
    UTabWidget *m_windowTab = nullptr;

    //QScrollBar *hscrollbar;
    //QScrollBar *vscrollbar;
};
#endif // UGRAPHICSVIEWHMI_H
