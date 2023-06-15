#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMap>
#include <QSet>
#include "AddrEditWidget.h"

#define WORKSPACE "/project/script"

class QsciScintilla;
class QsciLexerLua;
class QsciAPIs;
class QToolBar;
class QTextEdit;
class QTreeWidget;
class QComboBox;
class QGroupBox;
class QLineEdit;
class QTreeWidgetItem;
class QListWidget;
class QListWidgetItem;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    //书签数据
    struct BookMarkData
    {
      int iLineNumber;//书签行号
      int mhandle;//书签句柄
      QString sScriptName;//书签脚本名称
      QString sLineName;//书签脚本行内容
      bool operator ==(const BookMarkData &BD){
          if(mhandle == BD.mhandle && sScriptName == BD.sScriptName)
          //if(mhandle == BD.mhandle)
              return true;
          else
              return false;
      }
      bool operator <(const BookMarkData &BD){
          if(sScriptName < BD.sScriptName)
          {
              return true;
          }
          else if (sScriptName == BD.sScriptName) {
              if(mhandle < BD.mhandle)
              {
                  return true;
              }
          }
          return false;
      }
    };
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    //脚本编译检查
    bool Compile(const QString &text, QString &errCode);
    //初始化工具栏
    void InitToolBar();
    //初始化函数树
    void InitFuncTreeWidget();
    //初始化脚本编辑区widget
    void InitScriptEditWidget(QWidget *widget);
    //函数补全初始化
    void InitFunctionNameComplement(QWidget *widget);
    //设置变化标识
    void SetChangeFlag(QWidget * widget);
    //清除变化标识
    void ClearChangeFlag(QWidget * widget);
    //获取变化标识
    bool GetChangeFlag(QWidget * widget);
    //关闭事件函数
    void closeEvent(QCloseEvent *e);
    #if 0
    //事件过滤器，处理换行自动缩进等等
    bool eventFilter(QObject *watched, QEvent *event);
    #endif
    //打开指定脚本操作
    bool OpenScriptFromFile(const QString &file);
    //更新书签
    int UpdateBookMark(BookMarkData &BD, QsciScintilla* textEdit);
    //获取书签行名称
    QString GetBookMarkName(QsciScintilla *textEdit, int Row);
    //获取书签显示文本
    QString GetBookMarkText(const BookMarkData &BD);
    //获取书签唯一标识符
    QString GetBookMarkFlag(const BookMarkData &BD);
    //标签去重操作
    void BookmarkDeduplication();
    //刷新脚本书签
    void RefreshBookmarkOfScriptWindow(QsciScintilla* textEdit);
    //书签数据保存到配置文件
    void SaveBookmarks();
    //书签数据上载
    void OpenBookmarks();
    //启动时按上次会话配置
    void OpenCurrentSession();
    //保存会话配置
    void SaveCurrentSession();


public slots:
    //新建
    void newScriptSlot();
    //打开
    void openScriptSlot();
    //保存
    void saveScriptSlot();
    //保存所有
    void saveAllScriptSlot();
    //编译
    void compileSlot();
    //脚本类型变化
    void typeChangeSlot(int index);
    //函数树双击
    void doubleClicked(QTreeWidgetItem *item, int col);
    //关闭指定的脚本页面
    void closeScriptTab(int index);
    //打书签槽
    void bookmarkSlot(int margin, int line, Qt::KeyboardModifiers state);
    //更新书签
    void updateBookmarks();
    //书签双击槽函数
    void bookmarkDoubleClickedSlot(QListWidgetItem *item);

private:
    QWidget *mainW = nullptr;
    QTabWidget * m_maintabW;
    //编译显示
    QTextEdit * m_compileRet;
    //工具栏
    QToolBar * m_toolBar;
    //函数树，函数
    QTreeWidget * m_FuncTree;
    QMap<QString,QString> m_functionMap;
    QStringList m_funcStringList;
    //书签
    QListWidget * m_booksListWidget;
    QList<BookMarkData> m_bookMarks;
    QMap<QsciScintilla*,QList<BookMarkData> > m_bookmarkFlagOfDataMap;  //书签标识符对应书签数据
    //触发变量
    Protocol::AddrInfoForRW m_triggerVar;
    //新建脚本计数
    int m_newNumber = 0;
};

#endif // MAINWINDOW_H
