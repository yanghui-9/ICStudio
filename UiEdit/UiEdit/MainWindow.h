#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UToolbar.h"
#include "MainStatusBar.h"
#include "UTabWidget.h"

#define WORKSPACE "/project/scene"

class QTreeWidget;
class QToolBar;
class interface_gallery;
class interface_variable_table;
class CustomComponentsDlg;
class DisplayCustomComponentsDlg;
class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class UToolbar;
    friend class UTabWidget;
public:
    explicit MainWindow(std::shared_ptr<interface_comm> comm,QWidget *parent = nullptr);
    ~MainWindow();
    //初始化
    void initUI();
    //切换模式，编辑和运行模式，显示隐藏设置
    void SwitchMode(bool flag);

    //设置变化标识
    void SetChangeFlag(QWidget * widget);
    //清除变化标识
    void ClearChangeFlag(QWidget * widget);
    //获取变化标识
    bool GetChangeFlag(QWidget * widget);
    //获取编辑or运行模式
    bool GetModeFalg()
    {
        return m_isEditMode;
    }
    //切换画面
    bool SwitchScene(const std::string &name){
        int iRet = -1;
        if(m_View)
        {
            m_View->setCurScene(QString::fromLocal8Bit( name.data() ));
            iRet = 0;
        }
        return iRet;
    }
    //自定义控件选择处理
    void CustomItemselectDeal(const QString &item);

protected:
    //初始化工具栏
    void InitToolBars();
    //初始化菜单栏
    void InitMenuBar();
    //启动时按上次会话配置
    void OpenCurrentSession();
    //保存会话配置
    void SaveCurrentSession();
    //打开指定画面文件
    bool OpenFromFile(const QString &file);
    //关闭事件函数
    void closeEvent(QCloseEvent *e);
    //退出
    bool onClose();
    //显示菜单
    void ShowMenu();
    //通过配置加载自定义组件工具栏
    void LoadCombineItems(const QString &sList);

public slots:
    //新建
    void newSlot();
    //打开
    void openSlot();
    //保存
    void saveSlot();
    //保存所有
    void saveAllSlot();
    //编译
    void compileSlot();
    //编辑时画面关闭
    void closeTabSlot(int index);
    //切换画面
    void tabSwitchSlot(int index);
    //自定义组件工具栏按下
    void dealCombineItemSlot();

private:
    //工具栏
    UToolbar * m_toolBar = nullptr;
    //中央视图
    UTabWidget * m_View = nullptr;
    //状态栏
    MainStatusBar * m_statusBar = nullptr;
    //文件工具栏
    QToolBar * m_fileToolBar;
    //设置工具栏
    QToolBar *m_settingToolbar;

    //新建id
    int m_newID = 0;

    //模式
    bool m_isEditMode = true;

    //自定义组件选择对话框
    CustomComponentsDlg * m_CustomComponentsDlg = nullptr;
    DisplayCustomComponentsDlg * m_DCustomComponentsDlg = nullptr;
    QToolBar *m_combineItemsToolbar = nullptr;
    QString m_customSelectItems;

public:
    //图库
    std::shared_ptr<interface_gallery> m_gallery;
    //变量表
    std::shared_ptr<interface_variable_table> m_varTable;
    //外设
    std::shared_ptr<interface_comm> m_comm;
};

#endif // MAINWINDOW_H
