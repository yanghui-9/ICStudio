#ifndef MACROEDITDIALOG_H
#define MACROEDITDIALOG_H

#include <QtGui>
#include <QDialog>
#include <QPlainTextEdit>
#include <QObject>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QSize>
#include <QWidget>
#include <QSyntaxHighlighter>
#include <QCompleter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QGridLayout>
#include <QTreeWidget>
#include <QMessageBox>

#include "../../basics_model/device_system/PropertyDialogGlobalWidget.h"
#include "../../basics_model/device_comm/linkParaInterface.h"


struct  _ADDRESS_{
    QString link;
    int     sta;
    QString block;
    QString no;
    int     index;
    QString value;
    int     length;
    int     type;
    _ADDRESS_()
    {
        sta = 0;
        index = 0;
        length = 0;
        type = 0;
    }
};


namespace Ui {
class macroEditDialog;
}

typedef enum{
    BROWSE,
    EDIT,
}editorMode;

class macroDoubleclickedEditDialog;
class macroEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit macroEditDialog(QWidget *parent = 0);
    ~macroEditDialog();

    void initWidget();
    void refreshUI();
    macroDoubleclickedEditDialog * m_macroE;
    QMap<QString,QString>m_links;

private slots:
    void on_new__clicked();
    void on_delecte__clicked();
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_exit__clicked();
    void on_listWidget_2_doubleClicked(const QModelIndex &index);
    void on_save__clicked();
    void deleteItem(QModelIndex index);
    void on_edit__clicked();
    void on_copy__clicked();
    void on_paste__clicked();
    void on_import__clicked();
    void on_export__clicked();
/*【功能添加】_添加“拓展组件”、“脚本功能”帮助文档链接_xuxuan_20180730_Begin*/
    void on_help__clicked();
/*【功能添加】_添加“拓展组件”、“脚本功能”帮助文档链接_xuxuan_20180730_End*/
    void on_varTable__clicked();
    void on_example_clicked();

private:
    Ui::macroEditDialog *ui;
    QStringList m_sucMacro;
    QStringList m_sucMacroPro;
    QStringList m_failMacro;
    QStringList m_failMacroPro;
    QStringList m_macroVarListPro;
    struct object * m_macroObj;
    struct object * m_macroVarListObj;
    QList<struct object *> m_sucsaveBitAddressList;
    QList<struct object *> m_failsaveBitAddressList;
    int m_idCount;
    QList <int> m_idList;
    int m_copyFlag;
    int m_copyId;
};

class treeItemDoubleclickedEditDialog : public QDialog
{
    Q_OBJECT
public:
    treeItemDoubleclickedEditDialog(QWidget *parent = 0);
    void initWidget();
    void refreshUI(QString pro, QMap<QString, QString> *links);
    QString m_parameter;
    QTextEdit * m_infoEdit;
    QGridLayout *m_mainLayout;
    QGridLayout *m_parameterLayout;

    QList<int> m_parItemTypeList;
    QList<int>m_parTypeList;
    QList<void *> m_parItemList;

    QMap<int,void *>m_parUseVarItemMap;
    QList<void *>m_parCheckBoxList;

    //QMap<int,void *>m_DefaultParItemMap;
    QList<void *>m_DefaultParCheckBoxList;

    QMap<QString,QString>*m_links;
    QComboBox * m_linkscombox;
    QComboBox * m_regcombox;
    /*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180809_Begin*/
    QString m_infoStr;
    /*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180809_End*/
public slots:
   void mySave();
   void UseVarcheck();
   void defaultcheck();
   void linkComboxChange(int);
   /*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180809_Begin*/
   void systemParInfoSlot(QString value);
   /*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180809_End*/
};

class triggerAddrTypeDialog : public QDialog
{
    Q_OBJECT
public:
    triggerAddrTypeDialog(QWidget *parent = 0);
    int m_select;
    QGridLayout *m_mainLayout;
    QCheckBox * m_bitCheck;
    QCheckBox * m_byteCheck;
public slots:
   void myOk();
   void typeChange(bool flag= false);
};

class valueTableDialog : public QDialog
{
    Q_OBJECT
public:
    valueTableDialog(QWidget *parent = 0);
    QGridLayout *m_mainLayout;
    QTableWidget * m_table;
    QPushButton * m_addPtn;
    QPushButton * m_deletePtn;
    QPushButton * m_okPtn;
    QPushButton * m_exitPtn;
    QVector <_ADDRESS_> m_vAddressPro;
    QStringList m_proList;
    void setPro(QStringList list1);
    QStringList getPro();
public slots:
   void _Ok();
   void _exit();
   void _addSlot();
   void _deleteSlot();
   void _addrEdit(int row,int col);
};

class CodeEditor;
class macroDoubleclickedEditDialog : public QDialog
{
    Q_OBJECT
public:
    macroDoubleclickedEditDialog(void *links,QWidget *parent = 0);
    ~macroDoubleclickedEditDialog();

    void initWidget();
    void initTreeW();
    void refreshUI(QString macroStr, QString macroStrPro,int flag = 0,struct object * saveBitObj = NULL);
    void setVarList(QStringList list);
    QString checkDataype(int index);
    QString mergerExpressionToMacro(QString Expression,QString macroStr);
    int checkAddrToIndex(int linkid,QString reg,QString &addr);
    bool dealMacroText(int flag, QString & Text, bool useFlag=0);
    struct object * m_wObj;
    QPushButton * m_comfrim;
    QPushButton * m_save;
    QPushButton * m_exit;
    QFont * m_font;
    QLabel * m_fontStyle;
    QComboBox * m_fontStyleCom;
    QLabel * m_fontSize;
    QComboBox * m_fontSizeCom;
    CodeEditor *m_configEditor;
    QTreeWidget *m_treeW;
    QTextEdit * m_treeE;
    QLabel * m_idlabe;
    QLineEdit * m_idedit;
    QLabel * m_namelabe;
    QLineEdit * m_nameedit;
    QLabel * m_typelabe;
    QComboBox * m_typebox;
    QLabel * m_cycletimelabe;
    QLabel * m_cycleTimeMs;
    QLabel * m_tigglabe;
    QPushButton * m_tiggb;
    QLineEdit * m_typeedit;
    QTextEdit * m_checkEdit;
    QCheckBox * m_saveControlBit;
    AddressWidget * m_saveControlAddressWidget;
    struct object * m_saveBitAddressObj;
    QLabel * m_triggerTypeLabel;
    QComboBox * m_triggerTypeCom;
    int m_flag;
    int m_cycleTime;
    QString m_triggText;
    QString m_text;
    QString m_textPro;
    QMap<QString,QString> m_funcPro;
     QMap<QString,QString>*m_links;
     QList<QString> m_tiggValueList;
     QList<QString> m_valueList;
     QList<QStringList> m_valueProList;
     int m_flagType;
     triggerAddrTypeDialog *m_triggerAddType;
public slots:
    void editOUT(QString, int);
    void myExit();
    void mySave();
    void compile();
    void itemP(QTreeWidgetItem *, int);
    void itemDoubleC(QTreeWidgetItem *, int);
    void macroTypeChange(int);
    void macroTiggAddressEdit();
    void textFontChange(int);
    void saveControlbitCheckboxChange(bool);
};



class LineNumberArea;
class MyHighLighter;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);
    void setMode(editorMode mode);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void keyPressEvent(QKeyEvent * e); //换行退格处理
    bool event(QEvent *e); //拦截处理QtoolTip消息，鼠标停留提示信息显示消息
    void mouseDoubleClickEvent(QMouseEvent * e);//拦截处理鼠标双击消息
    void mousePressEvent(QMouseEvent * e);
    int isExpression(QString str);
    bool checkTriggerAddressType(QString text);//触发脚本静态地址检查
    void compileMacro(QString text);
    QString m_TipText;
    int m_KeyReturnFlag;
    MyHighLighter *m_highL;
    void setCompleter(QCompleter *completer,QStringList list);//增加处理关键字，函数输入补齐操作
    QStringList m_wordlist;
    QString textUnderCursor();
    QString textBeforeCurrentCursor();//当前光标前的连体提示字符串
    QCompleter * m_c;
    QString m_period;//自动补全碰到.号时的处理

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);
    void insertCompletion(const QString& completion);
public slots:
    void wordListChange();
signals:
    void editOut(QString,int);
    void LuaResult(QString);

private:
    QWidget *lineNumberArea;
signals:
    void enteRules(const QString &,int &);
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        codeEditor = editor;
    }
    QSize sizeHint() const  {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor *codeEditor;
};

class MyHighLighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    MyHighLighter(QTextDocument *parent = 0);
    QStringList m_functionNameList;
    QStringList m_varNameList;
    QStringList m_valueList;
public slots:
    void EnterRuleSlot(const QStringList &,int &);
    int TipRuleSlot(QString &,int );
    void getFuncNameList();

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QVector<HighlightingRule> EnterRules;
    QVector<HighlightingRule> TipRules;
    QVector<QString> Tipstrs;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineKey;
    QTextCharFormat singleLineValue;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat varFormat;
signals:
    void wordListChange();
};


#endif // MACROEDITDIALOG_H
