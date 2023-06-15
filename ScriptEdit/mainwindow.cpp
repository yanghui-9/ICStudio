#include "mainwindow.h"
#include <QMessageBox>
#include <QToolBar>
#include <QAction>
#include <QTabWidget>
#include <QListWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QFontComboBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QDataStream>
#include <QApplication>
#include <QStyleFactory>
#include <QTextEdit>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>
#include <Qsci/qsciapis.h>

#include "lua/src/lua.hpp"

#include "FunctionEditDialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setMinimumSize(1024,600);//设置最小size
    showMaximized();//最大化显示
    setWindowTitle("ScriptEditor");
    //样式处理
    if(-1 != QStyleFactory::keys().indexOf("Windows"))
        qApp->setStyle(QStyleFactory::create("Windows"));

    //tab页面
    m_maintabW = new QTabWidget(this);
    m_maintabW->setMovable(true);
    //m_maintabW->setTabBarAutoHide(true);
    m_maintabW->setTabShape(QTabWidget::Triangular);
    m_maintabW->setTabsClosable(true);
    connect(m_maintabW,SIGNAL(tabCloseRequested(int)),this,SLOT(closeScriptTab(int)));

    //设置显示
    mainW = new QWidget(this);
    QVBoxLayout * mainlayout = new QVBoxLayout(mainW);
    QHBoxLayout * hlayout = new QHBoxLayout(mainW);
    QVBoxLayout * vlayout = new QVBoxLayout(mainW);
    m_FuncTree = new QTreeWidget(this);
    m_booksListWidget = new QListWidget(this);
    connect(m_booksListWidget,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(bookmarkDoubleClickedSlot(QListWidgetItem *)));
    vlayout->addWidget(m_FuncTree);
    vlayout->addWidget(m_booksListWidget);
    hlayout->addWidget(m_maintabW,8);
    //hlayout->addWidget(m_FuncTree,1);
    hlayout->addLayout(vlayout,1);
    mainlayout->addLayout(hlayout,10);

    //编译检查结果显示
    m_compileRet = new QTextEdit(mainW);
    mainlayout->addWidget(m_compileRet,1);

    //中心显示区
    mainW->setLayout(mainlayout);

    //加载文件之前界面无法编辑
    //mainW->setEnabled(false);

    //设置显示
    setCentralWidget(mainW);

    //初始化工具栏
    InitToolBar();

    //初始化函数树
    InitFuncTreeWidget();

    //加载书签
    OpenBookmarks();

    //加载会话
    OpenCurrentSession();
}

MainWindow::~MainWindow()
{
    //保存书签信息
    SaveBookmarks();
    //保存会话信息
    SaveCurrentSession();
}

bool MainWindow::Compile(const QString &text,QString &errorSrt)
{
    bool bRet = false;
    static lua_State *L = nullptr;
    L =  luaL_newstate();
    luaL_openlibs(L);
    if(luaL_loadstring(L,text.toLatin1().constData()) != 0)
    {
        QString errStr = QString(QByteArray(lua_tostring(L,-1)));
        if(-1 != errStr.indexOf("]:"))
        {
            errStr.remove(0,errStr.indexOf("]:")+1);
            if(errStr.contains("'=' or 'in' expected near '('"))
            {
                QStringList tmpList = text.split('\n');
                int lineNum = errStr.mid(1,errStr.indexOf(':',1)-1).toInt() - 1 ;
                if(lineNum >= 0 && tmpList.at(lineNum).indexOf("for") != -1)
                    errStr = QString(":%1: ").arg(lineNum) + "variable table variables cannot be used in for loop conditional statements.";
            }
            errStr = "Line" + errStr;
        }
        errorSrt = errStr;
    }
    else
    {
        errorSrt = "no error";
        bRet = true;
    }
    lua_close(L);
    return bRet;
}

void MainWindow::InitToolBar()
{
    m_toolBar = new QToolBar(this);

    //新建
    QAction * newAc = new QAction(this);
    newAc->setIcon(QIcon(":/image/new.png"));
    newAc->setText("new");
    connect(newAc,SIGNAL(triggered()),this,SLOT(newScriptSlot()));
    m_toolBar->addAction(newAc);

    //打开文件
    QAction * openAc = new QAction(this);
    openAc->setIcon(QIcon(":/image/open.png"));
    openAc->setText("open");
    connect(openAc,SIGNAL(triggered()),this,SLOT(openScriptSlot()));
    m_toolBar->addAction(openAc);

    //保存
    QAction *saveAc = new QAction(this);
    saveAc->setIcon(QIcon(":/image/save.png"));
    saveAc->setText("save");
    connect(saveAc,SIGNAL(triggered()),this,SLOT(saveScriptSlot()));
    m_toolBar->addAction(saveAc);

    //保存所有
    QAction *saveAsAc = new QAction(this);
    saveAsAc->setIcon(QIcon(":/image/saveAll.png"));
    saveAsAc->setText("saveAll");
    connect(saveAsAc,SIGNAL(triggered()),this,SLOT(saveAllScriptSlot()));
    m_toolBar->addAction(saveAsAc);

    //编译
    QAction * compileAC = new QAction(this);
    compileAC->setIcon(QIcon(":/image/compile.png"));
    compileAC->setText("compile");
    connect(compileAC,SIGNAL(triggered()),this,SLOT(compileSlot()));
    m_toolBar->addAction(compileAC);

    //设置工具栏
    addToolBar(m_toolBar);

    //初始化快捷键
    saveAc->setShortcut(QKeySequence("Ctrl+S"));//保存
}

void MainWindow::InitFuncTreeWidget()
{
    //1.初始化
    m_FuncTree->setHeaderHidden(true);
    QTreeWidgetItem * topItem = new QTreeWidgetItem();
    topItem->setText(0,QString::fromLocal8Bit("功能"));
    m_FuncTree->addTopLevelItem(topItem);//设置顶层项
    m_FuncTree->setColumnCount(1);//两列
    m_FuncTree->expandAll();//始终展开
    connect(m_FuncTree,SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),this,SLOT(doubleClicked(QTreeWidgetItem *, int)));

    //读配置文件
    QDir dir2("system/script_function");
    QFileInfoList file_list = dir2.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    for (int j = 0; j < file_list.size(); ++j)
    {
        QFile file(file_list.at(j).filePath());
        if(file.open(QIODevice::ReadOnly))
        {
            m_functionMap.insert(file_list.at(j).fileName(),file.readAll());
            //添加一项
            QTreeWidgetItem *item =  new QTreeWidgetItem(topItem);
            item->setText(0,file_list.at(j).fileName());
            file.close();
            //名称保存，新增tab使用
            m_funcStringList.append(file_list.at(j).fileName());
        }
    }
}

void MainWindow::InitScriptEditWidget(QWidget * widget)
{
    QHBoxLayout * mainLayout = new QHBoxLayout(widget);

    QsciScintilla *editor = new QsciScintilla(widget);
    QsciLexerLua *textLexer = new QsciLexerLua();
    editor->setLexer(textLexer);
    editor->installEventFilter(this);
    widget->setProperty("QsciScintilla",QVariant::fromValue<void *>(editor));
    widget->setProperty("QsciScintilla_Lexer",QVariant::fromValue<void *>(textLexer));
    //监控脚本内容变化
    connect(editor,&QsciScintilla::textChanged,[=](){
        SetChangeFlag(m_maintabW->currentWidget());
    });
    //脚本内容变化触发书签变化
    connect(editor,SIGNAL(textChanged()),this,SLOT(updateBookmarks()));
    //1.行号显示
    editor->setMarginType(0, QsciScintilla::NumberMargin);
    editor->setMarginLineNumbers(0, true);
    editor->setMarginWidth(0, 30);
    //2.设置文档的编码格式为 “utf8” ，换行符为
    editor->setUtf8(true);
    editor->setEolMode(QsciScintilla::EolWindows);//文件中的每一行都以EOL字符结尾（换行符为 \r \n）
    //3.设置括号匹配模式
    editor->setUnmatchedBraceBackgroundColor(Qt::blue);
    editor->setBraceMatching(QsciScintilla::StrictBraceMatch);
    //4.设置代码自动折叠区域
    editor->setFolding(QsciScintilla::PlainFoldStyle);
    editor->setMarginWidth(2,12);
    //5.设置代码折叠和展开时的页边标记 - +
    editor->markerDefine(QsciScintilla::Minus, QsciScintilla::SC_MARKNUM_FOLDEROPEN);
    editor->markerDefine(QsciScintilla::Plus, QsciScintilla::SC_MARKNUM_FOLDER);
    editor->markerDefine(QsciScintilla::Minus, QsciScintilla::SC_MARKNUM_FOLDEROPENMID);
    editor->markerDefine(QsciScintilla::Plus, QsciScintilla::SC_MARKNUM_FOLDEREND);
    //6.设置代码折叠后，+ 的颜色FFFFFF
    //editor->setMarkerBackgroundColor(QColor("#FFBCBC"), QsciScintilla::SC_MARKNUM_FOLDEREND);
    //editor->setMarkerForegroundColor(QColor("red"), QsciScintilla::SC_MARKNUM_FOLDEREND);
    //7.关键字补全
    editor->setAutoCompletionSource(QsciScintilla::AcsAll);//补全出现在所有位置
    editor->setAutoCompletionCaseSensitivity(false);//大小写敏感
    editor->setAutoCompletionThreshold(1);//输入两个字符就提示补全
    editor->setTabIndents(true); //编辑器将行首第一个非空格字符推送到下一个缩进级别
    editor->setAutoIndent(true);//插入新行时，自动缩进将光标推送到与前一个相同的缩进级别
    editor->SendScintilla(QsciScintilla::SCI_SETCODEPAGE, QsciScintilla::SC_CP_UTF8);
    mainLayout->addWidget(editor,10);

    //书签
    editor->setMarginType(1, QsciScintilla::SymbolMargin);
    editor->setMarginLineNumbers(1, false);
    editor->setMarginWidth(1,20);
    editor->setMarginSensitivity(1, true); //设置是否可以显示断点
    //editor->setMarginsBackgroundColor(QColor("#bbfaae"));
    editor->setMarginMarkerMask(1, 0x02);
    connect(editor, SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)),this,
            SLOT(bookmarkSlot(int, int, Qt::KeyboardModifiers)));
    editor->markerDefine(QsciScintilla::Circle, 1);
    editor->setMarkerBackgroundColor(QColor("#0000FF"), 1);

    /////开始脚本属性布局
    QVBoxLayout * VLayoutType = new QVBoxLayout(widget);
    //字体和字体大小选择
    QGridLayout * fontlayout = new QGridLayout(widget);
    QGroupBox * fontBox = new QGroupBox(widget);
    QLabel * label = new QLabel(QString::fromLocal8Bit("字体:"),widget);
    QFontComboBox * fontFBox = new QFontComboBox(widget);
    fontFBox->setFont(QFont(fontFBox->font().family(),9));
    fontFBox->setObjectName("font_family_combox");
    QLabel * sizelabel = new QLabel(QString::fromLocal8Bit("字体大小:"),widget);
    QComboBox * fontSzieBox = new QComboBox(widget);
    for(uint i = 0; i < 100;i++)
    {
        fontSzieBox->addItem(QString::number(i));
    }
    fontSzieBox->setObjectName("font_size_combox");
    fontlayout->addWidget(label,0,0);
    fontlayout->addWidget(fontFBox,0,1);
    fontlayout->addWidget(sizelabel,1,0);
    fontlayout->addWidget(fontSzieBox,1,1);
    fontBox->setLayout(fontlayout);
    VLayoutType->addWidget(fontBox);
    //绑定字体变化生效
    connect(fontFBox,&QComboBox::currentTextChanged,[=](const QString &text){
     reinterpret_cast<QsciLexerLua*>(m_maintabW->currentWidget()->property("QsciScintilla_Lexer").value<void*>())->setFont(
                    QFont(text,m_maintabW->currentWidget()->findChild<QComboBox*>("font_size_combox")->currentText().toInt()));
        SetChangeFlag(m_maintabW->currentWidget());
    }
    );
    connect(fontSzieBox,&QComboBox::currentTextChanged,[=](const QString &text){
     reinterpret_cast<QsciLexerLua*>(m_maintabW->currentWidget()->property("QsciScintilla_Lexer").value<void*>())->setFont(
                    QFont(m_maintabW->currentWidget()->findChild<QComboBox*>("font_family_combox")->currentText(),text.toInt()));
        SetChangeFlag(m_maintabW->currentWidget());
    }
    );

    //脚本类型选择
    QGroupBox * Box0 = new QGroupBox(widget);
    QLabel * labelType = new QLabel(QString::fromLocal8Bit("脚本类型:"),Box0);
    QComboBox * m_typeComBox = new QComboBox(widget);
    m_typeComBox->setObjectName("script_type_combox");
    m_typeComBox->addItem(QString::fromLocal8Bit("触发脚本"));
    m_typeComBox->addItem(QString::fromLocal8Bit("循环脚本"));
    m_typeComBox->addItem(QString::fromLocal8Bit("开机脚本"));
    m_typeComBox->addItem(QString::fromLocal8Bit("普通脚本"));

    connect(m_typeComBox,SIGNAL(currentIndexChanged(int)),this,SLOT(typeChangeSlot(int)));
    QHBoxLayout *HLayout0 = new QHBoxLayout(Box0);
    HLayout0->addWidget(labelType);
    HLayout0->addWidget(m_typeComBox);
    Box0->setLayout(HLayout0);
    VLayoutType->addWidget(Box0);

    //触发地址选择
    QGroupBox *m_triggerVarBox = new QGroupBox(widget);
    m_triggerVarBox->setObjectName("trigger_box");
    labelType = new QLabel(QString::fromLocal8Bit("触发变量:"),m_triggerVarBox);
    AddrEditWidget *m_addrWidget = new AddrEditWidget(widget);
    m_addrWidget->setObjectName("script_trigger_AddrEditWidget");
    HLayout0 = new QHBoxLayout(m_triggerVarBox);
    HLayout0->addWidget(labelType);
    HLayout0->addWidget(m_addrWidget);
    m_triggerVarBox->setLayout(HLayout0);
    VLayoutType->addWidget(m_triggerVarBox);
    connect(m_addrWidget,&AddrEditWidget::Changed,[=](const QString&){
        SetChangeFlag(m_maintabW->currentWidget());
    });

    //循环时间选择
    QGroupBox *m_cycleTimeBox = new QGroupBox(widget);
    m_cycleTimeBox->setObjectName("cycle_box");
    labelType = new QLabel(QString::fromLocal8Bit("循环时间:"),m_cycleTimeBox);
    QLineEdit* m_cycleTimeEdit = new QLineEdit(widget);
    connect(m_cycleTimeEdit,&QLineEdit::textChanged,[=](const QString&){
        SetChangeFlag(m_maintabW->currentWidget());
    });
    m_cycleTimeEdit->setObjectName("script_cycleTime_lineedit");
    HLayout0 = new QHBoxLayout(m_cycleTimeBox);
    HLayout0->addWidget(labelType);
    HLayout0->addWidget(m_cycleTimeEdit);
    m_cycleTimeBox->setLayout(HLayout0);
    m_cycleTimeBox->setVisible(false);
    VLayoutType->addWidget(m_cycleTimeBox);

    VLayoutType->setSpacing(15);
    VLayoutType->addSpacerItem(new QSpacerItem(1,1000));
    mainLayout->addLayout(VLayoutType,1);

    widget->setLayout(mainLayout);
}

void MainWindow::InitFunctionNameComplement(QWidget * widget)
{
    QsciAPIs *m_apis = new QsciAPIs(reinterpret_cast<QsciLexerLua*>(widget->property("QsciScintilla_Lexer").value<void*>()));
    foreach (auto name, m_funcStringList) {
        m_apis->add(name);
    }
    m_apis->prepare();
}

void MainWindow::SetChangeFlag(QWidget *widget)
{
    int index = m_maintabW->indexOf(widget);
    if(-1 != index )
    {
        if(!m_maintabW->tabText(index).contains('*'))
            m_maintabW->setTabText(index,m_maintabW->tabText(index)+"*");
    }
}

void MainWindow::ClearChangeFlag(QWidget *widget)
{
    int index = m_maintabW->indexOf(widget);
    if(-1 != index )
    {
        if(m_maintabW->tabText(index).contains('*'))
            m_maintabW->setTabText(index,m_maintabW->tabText(index).remove('*'));
    }
}

bool MainWindow::GetChangeFlag(QWidget *widget)
{
    int index = m_maintabW->indexOf(widget);
    if(-1 != index )
    {
        if(!m_maintabW->tabText(index).contains('*'))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    bool bHaveNoSaveFile = false;
    for(int i = 0; i < m_maintabW->count(); ++i)
    {
        if(GetChangeFlag(m_maintabW->widget(i)))
        {
            bHaveNoSaveFile = true;
            break;
        }
    }
    if(bHaveNoSaveFile)
    {
        if(QMessageBox::Yes == QMessageBox::question(this,"Tips","There are currently unsaved files. Are you sure to exit?"))
        {//确认关闭，不保存
            e->accept();
        }
        else
        {//不关闭
            e->ignore();
        }
    }
}

bool MainWindow::OpenScriptFromFile(const QString &file)
{
    QFile dFile(file);
    if(dFile.open(QIODevice::ReadOnly))
    {
        //初始控件
        QWidget * curWidget;
        if(m_maintabW->tabText(m_maintabW->currentIndex()).contains("NEW"))
        {//当前待保存脚本为新建脚本
            curWidget = m_maintabW->currentWidget();
        }
        else
        {
            curWidget = new QWidget(this);
            InitScriptEditWidget(curWidget);
            InitFunctionNameComplement(curWidget);
        }

        QDataStream dataS(&dFile);
        //字体属性
        QString fontFamily;
        dataS >>fontFamily;
        int fontSize;
        dataS >>fontSize;
        //脚本类型
        int type;
        dataS >>type;
        //触发数据
        int triggerVarType;//触发变量模式
        dataS >> triggerVarType;
        QString linkName;//连接名称
        dataS >> linkName;
        QString reg;//寄存器名称
        dataS >> reg;
        QString sIndex;//索引字符串
        dataS >> sIndex;
        uint64_t Index;//索引
        dataS >> Index;
        int dataType;//数据类型
        dataS >> dataType;
        //循环时间
        int cycleTime;
        dataS >> cycleTime;
        //脚本内容
        QString text;;
        dataS >> text;

        dFile.close();

        //添加tab页
        m_maintabW->addTab(curWidget,file.split('/').last());
        //切换到新增的页面
        m_maintabW->setCurrentIndex(m_maintabW->count()-1);

        //数据初始化到界面
        //字体属性
        curWidget->findChild<QComboBox*>("font_family_combox")->setCurrentText(fontFamily);
        curWidget->findChild<QComboBox*>("font_size_combox")->setCurrentText(QString::number(fontSize));
        //脚本类型
        curWidget->findChild<QComboBox*>("script_type_combox")->setCurrentIndex(type);
        //触发变量
        m_triggerVar.linkName = linkName.toLocal8Bit().data();
        m_triggerVar.reg = reg.toLocal8Bit().data();
        m_triggerVar.sIndex = sIndex.toLocal8Bit().data();
        m_triggerVar.index = Index;
        m_triggerVar.dataType = static_cast<Protocol::Protocol_DataType>( dataType );
        curWidget->findChild<AddrEditWidget*>("script_trigger_AddrEditWidget")->InitData(m_triggerVar,triggerVarType);

        //循环时间
        curWidget->findChild<QLineEdit*>("script_cycleTime_lineedit")->setText(QString::number( cycleTime ));

        //脚本文本
        //curWidget->findChild<QsciScintilla*>("scriptr_editor")->setText(text);
        reinterpret_cast<QsciScintilla*>(curWidget->property("QsciScintilla").value<void*>())->setText(text);

        //清除变化标识
        ClearChangeFlag(curWidget);

        //更新脚本窗口的名称属性
        reinterpret_cast<QsciScintilla*>(curWidget->property("QsciScintilla").value<void*>())->setProperty(
                    "ScriptName",m_maintabW->tabText(m_maintabW->currentIndex()));
        //刷新书签
        updateBookmarks();

        //更新当前操作文档
        curWidget->setProperty("file",file);

        //添加书签
        RefreshBookmarkOfScriptWindow(reinterpret_cast<QsciScintilla*>(curWidget->property("QsciScintilla").value<void*>()));

        return true;
    }
    return false;
}

#if 0
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(QEvent::KeyPress ==  event->type())
    {
        QKeyEvent * keyEvent = dynamic_cast<QKeyEvent *>(event);
        static QString lastInput;
        if(keyEvent && keyEvent->text() == "\r")
        {
            if(lastInput == "{")
            {
            //    isspace(SendEditor(SCI_GETCHARAT,pos-2));
            }
        }
    }
    return false;
}
#endif

void MainWindow::newScriptSlot()
{
    //新建一个脚本编辑页面
    QWidget * curWidget = new QWidget(this);
    InitScriptEditWidget(curWidget);
    InitFunctionNameComplement(curWidget);
    m_maintabW->addTab(curWidget,"NEW"+QString::number(m_newNumber++));
    //切换到新增的页面
    m_maintabW->setCurrentIndex(m_maintabW->count()-1);

    //数据初始化到界面
    //字体属性
    curWidget->findChild<QComboBox*>("font_family_combox")->setCurrentText("Arial");
    curWidget->findChild<QComboBox*>("font_size_combox")->setCurrentText(QString::number(16));
    //脚本类型
    curWidget->findChild<QComboBox*>("script_type_combox")->setCurrentIndex(1);
    //触发变量
    m_triggerVar.linkName = "";
    m_triggerVar.reg = "";
    m_triggerVar.sIndex = "";
    m_triggerVar.index = 0;
    m_triggerVar.dataType = Protocol::bit;
    curWidget->findChild<AddrEditWidget*>("script_trigger_AddrEditWidget")->InitData(m_triggerVar,0);
    //循环时间
    curWidget->findChild<QLineEdit*>("script_cycleTime_lineedit")->setText("500");
    //脚本文本
    //curWidget->findChild<QsciScintilla*>("scriptr_editor")->setText(text);
    //reinterpret_cast<QsciScintilla*>(curWidget->property("QsciScintilla").value<void*>())->setText("");
    //更新脚本窗口的名称属性
    reinterpret_cast<QsciScintilla*>(curWidget->property("QsciScintilla").value<void*>())->setProperty("ScriptName",m_maintabW->tabText(m_maintabW->currentIndex()));
#if 0
    //判断是否保存当前
    if(!m_curFile.isEmpty())
    {//有正在操作的脚本文件，判断是否保存
       if(QMessageBox::Ok == QMessageBox::question(this,"Tips","Do you currently need to save?"))
       {
           saveScriptSlot();
       }
    }
#endif
}

void MainWindow::openScriptSlot()
{
    //先判断当前如果是new，编辑器是否存在文本
    if(m_maintabW->tabText(m_maintabW->currentIndex()).contains("NEW"))
    {//当前待保存脚本为新建脚本
        QString contend =  (reinterpret_cast<QsciScintilla*>(m_maintabW->currentWidget()->property("QsciScintilla").value<void *>()))->text();
        if(!contend.isEmpty())
        {
            //有正在操作的脚本文件，判断是否保存
            if(QMessageBox::Yes == QMessageBox::question(this,"Tips","Do you currently need to save?"))
            {
                saveScriptSlot();
            }
        }
    }
    //开始打开文件操作
    QString file = QFileDialog::getOpenFileName(this,"Open",QApplication::applicationDirPath()+WORKSPACE,"Files (*.script)");
    if (file.isNull())
    {
        return;
    }
    else
    {
        QString scriptName = file.split('/').last();
        //判断脚本是否打开过
        for (int i = 0; i < m_maintabW->count() ;++i) {
           if(m_maintabW->tabText(i) == scriptName)
           {//已经打开过
               return;
           }
        }
        OpenScriptFromFile(file);
    }
}

void MainWindow::saveScriptSlot()
{
    static QString m_curFile;
    if(!mainW->isEnabled())
    {
        return;
    }
    if(m_maintabW->tabText(m_maintabW->currentIndex()).contains("NEW"))
    {//当前待保存脚本为新建脚本
        QString file = QFileDialog::getSaveFileName(this,"Open",QApplication::applicationDirPath()+WORKSPACE,"Files (*.script)");
        if (file.isNull())
        {
            return;
        }
        m_curFile = file;
        //修改tab名称
        m_maintabW->setTabText(m_maintabW->currentIndex(),file.split('/').last());
        //更新脚本窗口的名称属性
        reinterpret_cast<QsciScintilla*>(m_maintabW->currentWidget()->property("QsciScintilla").value<void*>())->setProperty(
                    "ScriptName",m_maintabW->tabText(m_maintabW->currentIndex()));
        //刷新书签
        updateBookmarks();
    }
    else
    {
       m_curFile = m_maintabW->currentWidget()->property("file").toString();
    }
    QFile dFile(m_curFile);
    if(dFile.open(QIODevice::WriteOnly))
    {
        QWidget * curWidget = m_maintabW->currentWidget();
        ClearChangeFlag(curWidget);//先清除变化标识符
        QDataStream dataS(&dFile);
        //字体属性
        QString fontfamily = curWidget->findChild<QComboBox*>("font_family_combox")->currentText();
        dataS << fontfamily;
        int size = curWidget->findChild<QComboBox*>("font_size_combox")->currentText().toInt();
        dataS << size;
        //脚本类型
        int type = curWidget->findChild<QComboBox*>("script_type_combox")->currentIndex();
        dataS << type;
        //触发数据
        int triggerVarType = curWidget->findChild<AddrEditWidget*>("script_trigger_AddrEditWidget")->m_typeCombox->currentIndex();//触发变量模式
        dataS << triggerVarType;
        QString linkName = curWidget->findChild<AddrEditWidget*>("script_trigger_AddrEditWidget")->m_linkenameEdit->text();//连接名称
        dataS << linkName;
        QString reg = curWidget->findChild<AddrEditWidget*>("script_trigger_AddrEditWidget")->m_regEdit->text();//寄存器名称
        dataS << reg;
        QString sIndex = curWidget->findChild<AddrEditWidget*>("script_trigger_AddrEditWidget")->m_indexEdit->text();//索引字符串
        dataS << sIndex;
        uint64_t Index = curWidget->findChild<AddrEditWidget*>("script_trigger_AddrEditWidget")->m_indexEdit->text().toUInt();//索引
        dataS << Index;
        int dataType = curWidget->findChild<AddrEditWidget*>("script_trigger_AddrEditWidget")->m_dataTypeCombox->currentIndex();//数据类型
        dataS << dataType;
        //循环时间
        int cycleTime = curWidget->findChild<QLineEdit*>("script_cycleTime_lineedit")->text().toInt();
        dataS << cycleTime;
        //脚本内容
        QString text = reinterpret_cast<QsciScintilla*>(curWidget->property("QsciScintilla").value<void*>())->text();
        dataS << text;

        dFile.close();
    }
}

void MainWindow::saveAllScriptSlot()
{
    for(int i = 0; i < m_maintabW->count(); ++i)
    {
      m_maintabW->setCurrentIndex(i);
      saveScriptSlot();
    }
}

void MainWindow::compileSlot()
{
    if(!mainW->isEnabled())
    {
        return;
    }
    QString err;
    QWidget * curWidget = m_maintabW->currentWidget();
    Compile(reinterpret_cast<QsciScintilla*>(curWidget->property("QsciScintilla").value<void*>())->text(),err);
    m_compileRet->setText(err);
}

void MainWindow::typeChangeSlot(int index)
{
    if(0 == index)
    {//触发
       m_maintabW->currentWidget()->findChild<QGroupBox*>("trigger_box")->setVisible(true);
       m_maintabW->currentWidget()->findChild<QGroupBox*>("cycle_box")->setVisible(false);
    }
    else if (1 == index)
    {//循环
        m_maintabW->currentWidget()->findChild<QGroupBox*>("trigger_box")->setVisible(false);
        m_maintabW->currentWidget()->findChild<QGroupBox*>("cycle_box")->setVisible(true);
    }
    else
    {//
        m_maintabW->currentWidget()->findChild<QGroupBox*>("trigger_box")->setVisible(false);
        m_maintabW->currentWidget()->findChild<QGroupBox*>("cycle_box")->setVisible(false);
    }
    //设置变化标识
    SetChangeFlag(m_maintabW->currentWidget());
}

void MainWindow::doubleClicked(QTreeWidgetItem *item, int col)
{
    QMap<QString,QString>::iterator it =  m_functionMap.find(item->text(col));
    if(it != m_functionMap.end())
    {//查找到了
        FunciontEditDialog editD(this,it.key(),it.value());
        if(editD.exec() == QDialog::Accepted)
        {
            QWidget * curWidget = m_maintabW->currentWidget();
            if(curWidget)
            {
                reinterpret_cast<QsciScintilla*>(curWidget->property("QsciScintilla").value<void*>())->insert(editD.GetRusult());
            }
        }
    }
}

void MainWindow::closeScriptTab(int index)
{
    m_maintabW->setCurrentIndex(index);//切换到这个页面
    //判断页面是否变化
    if(GetChangeFlag(m_maintabW->widget(index)))
    {
        if(QMessageBox::Yes == QMessageBox::question(this,"Tips","Do you currently need to save?"))
        {
            saveScriptSlot();
        }
    }
    QWidget *widget =  m_maintabW->widget(index);
    m_maintabW->removeTab(index);
    widget->deleteLater();
}

void MainWindow::bookmarkSlot(int margin, int line, Qt::KeyboardModifiers state)
{
    //获取当前脚本窗口
    Q_UNUSED(state);
    QsciScintilla* textEdit = nullptr;
    QWidget * curWidget = m_maintabW->currentWidget();
    if(curWidget)
    {
        textEdit = reinterpret_cast<QsciScintilla*>(curWidget->property("QsciScintilla").value<void*>());
    }
    //开始处理书签
    if (1 == margin)
    {//书签
        BookMarkData BD;
        BD.sScriptName = m_maintabW->tabText(m_maintabW->currentIndex());
        BD.sLineName = GetBookMarkName(textEdit,line);
        BD.iLineNumber = line+1;
        if (textEdit->markersAtLine(line) != 0) {
            textEdit->markerDelete(line, 1);
            //做一些去掉断点的逻辑程序
            int row = m_bookMarks.indexOf(BD);
            if(-1 != row)
            {
                QListWidgetItem *item = m_booksListWidget->takeItem(row);
                delete item;
                m_bookMarks.removeAt(row);
                QMap<QsciScintilla*,QList<BookMarkData> >::Iterator it = m_bookmarkFlagOfDataMap.find(textEdit);
                if(it != m_bookmarkFlagOfDataMap.end())
                {
                    it.value().removeOne(BD);
                }
            }
        } else {
            BD.mhandle = textEdit->markerAdd(line, 1);
            //做一些增加断点的逻辑程序
            m_bookMarks.push_back(BD);
            m_booksListWidget->addItem(GetBookMarkText(BD));
            QMap<QsciScintilla*,QList<BookMarkData> >::Iterator it = m_bookmarkFlagOfDataMap.find(textEdit);
            if(it != m_bookmarkFlagOfDataMap.end())
            {
                it.value().push_back(BD);
            }
            else
            {
                QList<BookMarkData> BDSet;
                BDSet.push_back(BD);
                m_bookmarkFlagOfDataMap.insert(textEdit,BDSet);
            }
        }
    }
}

void MainWindow::updateBookmarks()
{
    QsciScintilla * textEditor = dynamic_cast<QsciScintilla *>( sender() );
    if(!textEditor)
    {
        textEditor = reinterpret_cast<QsciScintilla*>(m_maintabW->currentWidget()->property("QsciScintilla").value<void*>());
    }
    if(textEditor)
    {
        QMap<QsciScintilla*,QList<BookMarkData> >::iterator it = m_bookmarkFlagOfDataMap.find(textEditor);
        if(it != m_bookmarkFlagOfDataMap.end() )
        {
            foreach (auto BD, it.value())
            {
                if(-1 == UpdateBookMark(BD,textEditor))
                {//标签已经删除
                    it.value().removeOne(BD);
                }
            }
        }

#if 0
        QString scriptFlag;
        QMap<QString,BookMarkData>::Iterator FindIt;
         int line = textEditor->markerFindNext(0,0b11111);
         while(-1 != line)
         {
             scriptFlag = textEditor->property("ScriptName").toString()+QString::number(line+1);
             FindIt = m_bookmarkFlagOfDataMap.find(scriptFlag);
             if(FindIt != m_bookmarkFlagOfDataMap.end())
             {
                 if(-1 == UpdateBookMark(FindIt.value(),textEditor))
                 {//标签已经删除
                     m_bookmarkFlagOfDataMap.erase(FindIt);
                 }
             }
             line = textEditor->markerFindNext(line+1,0b11111);
         }
#endif
    }
}

void MainWindow::bookmarkDoubleClickedSlot(QListWidgetItem *item)
{
    int row =  m_booksListWidget->row(item);
    if(row < m_bookMarks.size())
    {
        //查找书签所指向的脚本是否已经打开
        int i = 0;
        for(; i < m_maintabW->count();++i)
        {
            if(m_maintabW->tabText(i) == m_bookMarks.at(row).sScriptName)
            {//切换到该脚本页面
                m_maintabW->setCurrentIndex(i);
                break;
            }
        }
        if(i == m_maintabW->count())
        {//没找到该脚本，默认打开该脚本
            QString filePath = QApplication::applicationDirPath()+WORKSPACE+"/"+m_bookMarks.at(row).sScriptName;
            if(!OpenScriptFromFile(filePath))
            {//未找到该脚本文件
                return;
            }
        }

        //获取当前脚本窗口
        QsciScintilla* textEdit = nullptr;
        QWidget * curWidget = m_maintabW->currentWidget();
        if(curWidget)
        {
            textEdit = reinterpret_cast<QsciScintilla*>(curWidget->property("QsciScintilla").value<void*>());
            textEdit->setFocus(Qt::MouseFocusReason);
            textEdit->setCursorPosition(m_bookMarks.at(row).iLineNumber-1,0);
        }
    }
}

int MainWindow::UpdateBookMark(MainWindow::BookMarkData &BD, QsciScintilla *textEdit)
{
   int row = m_bookMarks.indexOf(BD);
   if(-1 == row)
   {
       return -1;
   }
   QListWidgetItem * item = m_booksListWidget->item(row);
   if(item)
   {
       int newLineNumber = textEdit->markerLine(BD.mhandle)+1;
       if( 0 == newLineNumber )
       {//该行的标签不存在了，删除
           m_bookMarks.removeAt(row);
           QListWidgetItem *item = m_booksListWidget->takeItem(row);
           delete item;
           return -1;
       }
       //此次还需要判断标签是否重合了，得删除一个，后续加
       BD.iLineNumber = newLineNumber;
       BD.sLineName = GetBookMarkName(textEdit,BD.iLineNumber-1);
       BD.sScriptName = textEdit->property("ScriptName").toString();
       item->setText(GetBookMarkText(BD));
       m_bookMarks[row] = BD;
       return 0;
   }
   return -1;
}

inline QString MainWindow::GetBookMarkName(QsciScintilla* textEdit,int Row)
{
    return  textEdit->text(Row).left(32).remove('\n').remove('\r');
}

inline QString MainWindow::GetBookMarkText(const MainWindow::BookMarkData &BD)
{
    return BD.sScriptName+"  "+QString::number(BD.iLineNumber)+"\n"+BD.sLineName+"\n";
}

inline QString MainWindow::GetBookMarkFlag(const MainWindow::BookMarkData &BD)
{
    return BD.sScriptName+QString::number(BD.iLineNumber);
}

void MainWindow::BookmarkDeduplication()
{
//    QListWidget * m_booksListWidget;
//    QList<BookMarkData> m_bookMarks;
//    QMap<QsciScintilla*,QList<BookMarkData> > m_bookmarkFlagOfDataMap;  //书签标识符对应书签数据

//    for (int i = 0; i < m_bookMarks.size() ;++i) {

    //    }
}

void MainWindow::RefreshBookmarkOfScriptWindow(QsciScintilla *textEdit)
{
   QList<BookMarkData> BDList;
   for(int i = 0; i < m_bookMarks.size(); ++i )
   {
       QString tmps =  textEdit->property("ScriptName").toString();
       if(textEdit->property("ScriptName").toString() == m_bookMarks.at(i).sScriptName)
       {
          m_bookMarks[i].mhandle = textEdit->markerAdd(m_bookMarks[i].iLineNumber-1, 1);
          BDList.push_back(m_bookMarks.at(i));
       }
   }
   m_bookmarkFlagOfDataMap.insert(textEdit,BDList);
}

void MainWindow::SaveBookmarks()
{
    QString file = "project/script/bookmarks";
    QFile dFile(file);
    if(dFile.open(QIODevice::WriteOnly))
    {
        QDataStream dataS(&dFile);
        dataS << static_cast<int>( m_bookMarks.size() );
        foreach (auto BD, m_bookMarks) {
           dataS <<BD.sScriptName;
           dataS <<BD.iLineNumber;
           dataS <<BD.sLineName;
        }
        dFile.close();
    }
}

void MainWindow::OpenBookmarks()
{
    QString file = "project/script/bookmarks";
    QFile dFile(file);
    if(dFile.open(QIODevice::ReadOnly))
    {
        QDataStream dataS(&dFile);
        int size = 0;
        dataS >> size;
        BookMarkData BD;
        for( int i = 0; i < size; ++i)
        {
            dataS >>BD.sScriptName;
            dataS >>BD.iLineNumber;
            dataS >>BD.sLineName;
            m_bookMarks.push_back(BD);
            m_booksListWidget->addItem(GetBookMarkText(BD));
        }
        dFile.close();
    }
}

void MainWindow::OpenCurrentSession()
{
    QString file = "project/script/CurrentSession";
    QFile dFile(file);
    if(dFile.open(QIODevice::ReadOnly))
    {
        QDataStream dataS(&dFile);
        QString sScrptList;
        dataS >> sScrptList;
        int iCurScript;
        dataS >> iCurScript;
        //打开脚本文件
        QStringList sList = sScrptList.split('#');
        QString filePath;
        foreach (auto sName, sList) {
            filePath = QApplication::applicationDirPath()+WORKSPACE+"/"+ sName;
            OpenScriptFromFile(filePath);
        }
        //切换到当前
        m_maintabW->setCurrentIndex(iCurScript);
        dFile.close();
    }
}

void MainWindow::SaveCurrentSession()
{
    QString file = "project/script/CurrentSession";
    QFile dFile(file);
    if(dFile.open(QIODevice::WriteOnly))
    {
        QDataStream dataS(&dFile);
        QStringList sScrptList;
        for (int i = 0; i < m_maintabW->count(); ++i) {
            sScrptList.push_back(m_maintabW->tabText(i));
        }
        dataS << sScrptList.join("#");
        int iCurScript = m_maintabW->currentIndex();
        dataS << iCurScript;
        dFile.close();
    }
}
