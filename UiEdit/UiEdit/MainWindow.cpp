#include "MainWindow.h"
#include <QAction>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStyleFactory>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QPushButton>
#include <QToolBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>
#include "Gallery/Gallery.h"
#include "VariableTable/VariableTable.h"
#include "CustomComponentsDlg.h"
#include "DisplayCustomComponentsDlg.h"

#include "Tools/tools.h"

MainWindow::MainWindow(std::shared_ptr<interface_comm> comm, QWidget *parent) :
    QMainWindow(parent),m_comm(comm)
{
}

MainWindow::~MainWindow()
{
    //画面切换
    if(m_isEditMode)
    {
        disconnect(m_View,SIGNAL(currentChanged(int)),this,SLOT(tabSwitchSlot(int)));
    }
    //SaveCurrentSession();
    //delete ui;
}

void MainWindow::initUI()
{
    //创建图库对象
    m_gallery = std::make_shared<Gallery>();
    //创建变量表对象
    m_varTable = std::make_shared<VariableTable>();

    //模式选择编辑or运行
    if(QMessageBox::No == QMessageBox::question(this,QString::fromLocal8Bit("模式选择"),QString::fromLocal8Bit("是否进入编辑模式？")))
    {
        m_isEditMode = false;
        //设置无窗口边框
        setWindowFlag(Qt::FramelessWindowHint);
    }

   //设定窗口尺寸参数
    setMinimumSize(1600,900);//设置最小size
    showMaximized();//最大化显示

    //设置字体
    //qApp->setFont(QFont("Arial",16));

    //标题设置
    setWindowTitle("UiEditor");

    //样式处理
    if(-1 != QStyleFactory::keys().indexOf("Windows"))
        qApp->setStyle(QStyleFactory::create("Windows"));

    //状态栏
    if(m_isEditMode)
    {
        m_statusBar = new MainStatusBar(this);
        m_statusBar->retranslateUI();
        m_statusBar->refreshUIfromData();
        setStatusBar(m_statusBar);
    }

    //中心视图
    if(!m_isEditMode)
    {//运行模式
        m_View = new UTabWidget(this);
        //运行模式下，不显示tab表头
        m_View->tabBar()->hide();
        setCentralWidget(m_View);
    }
    else
    {//编辑模式
        QWidget * MainW = new QWidget(this);
        QVBoxLayout * MainLayout = new QVBoxLayout(MainW);
        m_View = new UTabWidget(this);
        //tab页面关闭
        connect(m_View,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTabSlot(int)));

        MainLayout->addWidget(m_View);
        MainW->setLayout(MainLayout);
        setCentralWidget(MainW);
    }


   //建立工具栏
    if(m_isEditMode)
    {
        InitToolBars();
    }

    //建立菜单栏
    if(m_isEditMode)
    {
        InitMenuBar();
    }

   //加载会话
   OpenCurrentSession();

   //画面切换
   if(m_isEditMode)
   {
       connect(m_View,SIGNAL(currentChanged(int)),this,SLOT(tabSwitchSlot(int)));
   }
}

void MainWindow::SwitchMode(bool flag)
{
    m_toolBar->SetToolHide(flag);
    m_statusBar->setVisible(flag);
    if(!flag)
    {
        m_View->tabBar()->hide();
    }
    else
    {
        m_View->tabBar()->show();
    }
}

void MainWindow::InitToolBars()
{
    m_fileToolBar = new QToolBar(QString::fromLocal8Bit("文件"),this);
    //新建
    QAction * newAc = new QAction(this);
    newAc->setIcon(QIcon(":/image/new.png"));
    newAc->setText(QString::fromLocal8Bit("新建"));
    connect(newAc,SIGNAL(triggered()),this,SLOT(newSlot()));
    m_fileToolBar->addAction(newAc);

    //打开文件
    QAction * openAc = new QAction(this);
    openAc->setIcon(QIcon(":/image/open.png"));
    openAc->setText(QString::fromLocal8Bit("打开"));
    connect(openAc,SIGNAL(triggered()),this,SLOT(openSlot()));
    m_fileToolBar->addAction(openAc);

    //保存
    QAction *saveAc = new QAction(this);
    saveAc->setIcon(QIcon(":/image/save.png"));
    saveAc->setText(QString::fromLocal8Bit("保存"));
    connect(saveAc,SIGNAL(triggered()),this,SLOT(saveSlot()));
    m_fileToolBar->addAction(saveAc);
    saveAc->setShortcut(QKeySequence("Ctrl+S"));

    //保存所有
    QAction *saveAsAc = new QAction(this);
    saveAsAc->setIcon(QIcon(":/image/saveall.png"));
    saveAsAc->setText(QString::fromLocal8Bit("保存所有"));
    connect(saveAsAc,SIGNAL(triggered()),this,SLOT(saveAllSlot()));
    m_fileToolBar->addAction(saveAsAc);

    //编译
    QAction * compileAC = new QAction(this);
    compileAC->setIcon(QIcon(":/image/compile.png"));
    compileAC->setText(QString::fromLocal8Bit("编译"));
    connect(compileAC,SIGNAL(triggered()),this,SLOT(compileSlot()));
    m_fileToolBar->addAction(compileAC);

    //导入
    QAction * importAC = new QAction(this);
    importAC->setIcon(QIcon(":/image/import.png"));
    importAC->setText(QString::fromLocal8Bit("导入"));
    connect(importAC,SIGNAL(triggered()),this,SLOT(importProjectSlot()));
    m_fileToolBar->addAction(importAC);

    //导出
    QAction * exportAC = new QAction(this);
    exportAC->setIcon(QIcon(":/image/export.png"));
    exportAC->setText(QString::fromLocal8Bit("导出"));
    connect(exportAC,SIGNAL(triggered()),this,SLOT(exportProjectSlot()));
    m_fileToolBar->addAction(exportAC);

    addToolBar(m_fileToolBar);

    //初始化图库和变量表和脚本库，工具栏
    m_settingToolbar = new QToolBar(QString::fromLocal8Bit("设置"),this);
    QAction * picAC = new QAction(this);
    picAC->setIcon(QIcon(":/image/gallery.png"));
    picAC->setText(QString::fromLocal8Bit("图库"));
    connect(picAC,&QAction::triggered,[this](){
       if(m_gallery) m_gallery->ShowImgSelectDialog(this);
       //图库编辑之后，控件对应的图片可能发生变化，刷新当前画面
       m_View->curScene()->reflashAllItem();
    });
    m_settingToolbar->addAction(picAC);

    //变量表
    QAction * varC = new QAction(this);
    varC->setIcon(QIcon(":/image/variables.png"));
    varC->setText(QString::fromLocal8Bit("数据中心"));
    connect(varC,&QAction::triggered,[this](){
       if(m_varTable) m_varTable->ShowVarSelectDialog(this);
    });
    m_settingToolbar->addAction(varC);

    //脚本
    QAction * scriptC = new QAction(this);
    scriptC->setIcon(QIcon(":/image/script.png"));
    scriptC->setText(QString::fromLocal8Bit("脚本"));
    connect(scriptC,&QAction::triggered,[](){
        //调用外部脚本编辑器
        QProcess::execute("ScriptEditor.exe");
    });
    m_settingToolbar->addAction(scriptC);

    //自定义组件配置
    QAction * combineC = new QAction(this);
    combineC->setIcon(QIcon(":/image/custom item setting.png"));
    combineC->setText(QString::fromLocal8Bit("自定义组件编辑"));
    connect(combineC,&QAction::triggered,[this](){
        if(!m_CustomComponentsDlg)
        {
            m_CustomComponentsDlg = new CustomComponentsDlg(this);
            m_CustomComponentsDlg->InitUI();
        }
        m_CustomComponentsDlg->SetSelectItems(m_customSelectItems);
        //m_CustomComponentsDlg->UShow();
        if(QDialog::Accepted == m_CustomComponentsDlg->exec())
        {
            m_customSelectItems = m_CustomComponentsDlg->GetSelectPic();
            this->LoadCombineItems(m_customSelectItems);
        }
    });
    m_settingToolbar->addAction(combineC);

    //自定义组件显示
    QAction * combineCD = new QAction(this);
    combineCD->setIcon(QIcon(":/image/custom item display.png"));
    combineCD->setText(QString::fromLocal8Bit("自定义组件显示"));
    connect(combineCD,&QAction::triggered,[this](){
        if(!m_DCustomComponentsDlg)
        {
            m_DCustomComponentsDlg = new DisplayCustomComponentsDlg(this,this);
            m_DCustomComponentsDlg->InitUI();
        }
        m_DCustomComponentsDlg->LoadCombineItems(m_customSelectItems);
        m_DCustomComponentsDlg->UShow();
    });
    m_settingToolbar->addAction(combineCD);

    //系统设置
    QAction * sysC = new QAction(this);
    sysC->setIcon(QIcon(":/image/system setting.png"));
    sysC->setText(QString::fromLocal8Bit("系统设置"));
    connect(sysC,&QAction::triggered,[](){
        //
    });
    m_settingToolbar->addAction(sysC);

    addToolBar(m_settingToolbar);


    //初始化控件工具栏
    addToolBarBreak();
    m_toolBar = new UToolbar(this);
    m_toolBar->Init();

    //初始化自定义组件工具栏
    addToolBarBreak();
    m_combineItemsToolbar = new QToolBar(QString::fromLocal8Bit("自定义控件"),this);
    addToolBar(Qt::RightToolBarArea,m_combineItemsToolbar);
}

void MainWindow::InitMenuBar()
{
    QMenuBar * menuBar = new QMenuBar(this);

    //文件操作菜单
    QMenu * fileMenu = new QMenu(QString::fromLocal8Bit("文件"),menuBar);
    QList<QAction*> fileAcs = m_fileToolBar->actions();
    for (const auto &ac : fileAcs) {
        fileMenu->addAction(ac);
    }
    menuBar->addMenu(fileMenu);

    //视图菜单
    QMenu * viewMenu = new QMenu(QString::fromLocal8Bit("视图"),menuBar);
    viewMenu->addAction(m_fileToolBar->toggleViewAction());
    viewMenu->addAction(m_settingToolbar->toggleViewAction());
    QList<QAction *> aclist = m_toolBar->GetAclist();
    for (const auto &ac : aclist) {
       viewMenu->addAction(ac);
    }
    viewMenu->addAction(m_combineItemsToolbar->toggleViewAction());
    menuBar->addMenu(viewMenu);

    //设置菜单
    QMenu * settingMenu = new QMenu(QString::fromLocal8Bit("设置"),menuBar);
    QList<QAction*> settingAcs = m_settingToolbar->actions();
    for (const auto &ac : settingAcs) {
        settingMenu->addAction(ac);
    }
    menuBar->addMenu(settingMenu);

    //帮助
    QMenu * helpMenu = new QMenu(QString::fromLocal8Bit("帮助"),menuBar);
    menuBar->addMenu(helpMenu);

    setMenuBar(menuBar);
}

void MainWindow::newSlot()
{
    QString SName = QString("NEW%1").arg(m_newID++,3,10,QChar('0'));
    UWidgetScene *scene = new UWidgetScene(UTabWidget::Scene,SName,m_View);
    m_View->addScene(SName,scene);
    m_View->setCurScene(SName);
}

void MainWindow::openSlot()
{
    //先判断当前如果是new，编辑器是否存在文本
    if(m_View->tabText(m_View->currentIndex()).contains("NEW"))
    {//当前待保存脚本为新建脚本
        if(m_View->m_sceneList.size() > 0)
        {//当前画面有控件
            //有正在操作的脚本文件，判断是否保存
            if(QMessageBox::Yes == QMessageBox::question(this,"Tips","Do you currently need to save?"))
            {
                saveSlot();
            }
        }
    }
    //开始打开文件操作
    QString file = QFileDialog::getOpenFileName(this,"Open",QApplication::applicationDirPath()+WORKSPACE,"Files (*.scene)");
    if (file.isNull())
    {
        return;
    }
    else
    {
        QString sName = file.split('/').last();
        //判断脚本是否打开过
        for (int i = 0; i < m_View->count() ;++i) {
           if(m_View->tabText(i) == sName)
           {//已经打开过
               return;
           }
        }
        OpenFromFile(file);
    }
}

void MainWindow::saveSlot()
{
    static QString m_curFile;
    if(m_View->tabText(m_View->currentIndex()).contains("NEW"))
    {//当前待保存脚本为新建脚本
        QString file = QFileDialog::getSaveFileName(this,"Open",QApplication::applicationDirPath()+WORKSPACE,"Files (*.scene)");
        if (file.isNull())
        {
            return;
        }
        m_curFile = file;
        //修改tab名称
        m_View->setTabText(m_View->currentIndex(),file.split('/').last().split('.').first());
    }
    else
    {
       m_curFile = m_View->currentWidget()->property("file").toString();
    }
    QFile dFile(m_curFile);
    if(dFile.open(QIODevice::WriteOnly))
    {
        UWidgetScene * curWidget = qobject_cast<UWidgetScene *>(m_View->currentWidget());
        if(curWidget)
        {
          ClearChangeFlag(curWidget);//先清除变化标识符
          QString SData;
          curWidget->saveSceneItem(SData);
          dFile.write(SData.toUtf8());
        }
        dFile.close();
    }
}

void MainWindow::saveAllSlot()
{
    for(int i = 0; i < m_View->count(); ++i)
    {
      m_View->setCurrentIndex(i);
      saveSlot();
    }
}

void MainWindow::compileSlot()
{
    //
}

void MainWindow::closeTabSlot(int index)
{
    m_View->setCurrentIndex(index);//切换到这个页面
    //判断页面是否变化
    if(GetChangeFlag(m_View->widget(index)))
    {
        if(QMessageBox::Yes == QMessageBox::question(this,"Tips","Do you currently need to save?"))
        {
            //saveScriptSlot();
            saveSlot();
        }
    }
    QWidget *widget =  m_View->widget(index);
    m_View->removeTab(index);
    widget->deleteLater();
}

void MainWindow::tabSwitchSlot(int index)
{
    m_View->setCurScene(m_View->tabText(index).remove('*'));
}

void MainWindow::dealCombineItemSlot()
{
    QAction * ac = qobject_cast<QAction *>(sender());
    if(ac)
    {
        CustomItemselectDeal(ac->text());
    }
}

void MainWindow::exportProjectSlot()
{
    //获取导出文件名称和路径
    QString sFileName =  QFileDialog::getSaveFileName(this, QStringLiteral("导出"),"","Files (*.pic)");
    if(sFileName.isEmpty())
    {
        return;
    }

    //先删除文件
    QFile::remove(sFileName);

    //开始导出
    QProcess myProcess;
    QStringList args;
    args.append("a");
    args.append(sFileName);
    args.append(".\\project");
    args.append("-r");
    args.append("-y");
    args.append("-pyangzezhou2023");
    myProcess.start("./7z.exe",args);
    myProcess.waitForFinished();
    QString sReturn = QString::fromUtf8( myProcess.readAllStandardOutput() );
    if(sReturn.contains("cannot open") || sReturn.contains("Errors:"))
    {
        QMessageBox::warning(this,QStringLiteral("导出"),QStringLiteral("导出失败"));
    }
}

void MainWindow::importProjectSlot()
{
    //获取导入文件名称和路径
    QString sFileName =  QFileDialog::getOpenFileName(this,QStringLiteral("导入"),"","Files (*.pic)");
    if(sFileName.isEmpty())
    {
        QMessageBox::warning(this,QStringLiteral("导入失败"),QStringLiteral("文件不存在"));
        return;
    }

    QProcess myProcess;
    QStringList args;
    args.append("x");
    args.append(sFileName);
    args.append("-o.\\project_Tmp");
    args.append("-r");
    args.append("-y");
    args.append("-pyangzezhou2023");
    myProcess.start("./7z.exe",args);
    myProcess.waitForFinished();
    QString sReturn = QString::fromUtf8( myProcess.readAllStandardOutput() );
    if(sReturn.contains("Can't open as archive") || sReturn.contains("Errors:"))
    {//导入失败
        QMessageBox::warning(this,QStringLiteral("导入失败"),QStringLiteral("工程解压失败"));
        return;
    }
    else
    {//导入成功
        QDir rmDir;
        rmDir.rmdir("./project");
        if(Tools_CopyDirectory("./project_Tmp/project","./project",true))
        {
            OpenCurrentSession();
            QMessageBox::information(this,QStringLiteral("导入"),QStringLiteral("导入成功"));
        }
    }
}

void MainWindow::OpenCurrentSession()
{
    QString file = "project/scene/CurrentSession";
    QFile dFile(file);
    if(dFile.open(QIODevice::ReadOnly))
    {
        QDataStream dataS(&dFile);
        QString sScrptList;
        dataS >> sScrptList;
        int iCurScript;
        dataS >> iCurScript;
        dataS >> m_customSelectItems;
        dFile.close();
        //打开脚本文件
        QStringList sList = sScrptList.split('#');
        QString filePath;
        foreach (auto sName, sList) {
            filePath = QApplication::applicationDirPath()+WORKSPACE+"/"+ sName +".scene";
            OpenFromFile(filePath);
        }
        //切换到当前
        m_View->setCurScene(m_View->tabText(iCurScript));
        //通过配置加载自定义控件
        LoadCombineItems(m_customSelectItems);
    }
}

void MainWindow::SaveCurrentSession()
{
    QString file = "project/scene/CurrentSession";
    QFile dFile(file);
    if(dFile.open(QIODevice::WriteOnly))
    {
        QDataStream dataS(&dFile);
        QStringList sScrptList;
        for (int i = 0; i < m_View->count(); ++i) {
            sScrptList.push_back(m_View->tabText(i));
        }
        dataS << sScrptList.join("#");
        int iCurScript = m_View->currentIndex();
        dataS << iCurScript;
        dataS << m_customSelectItems;
        dFile.close();
    }
}

bool MainWindow::OpenFromFile(const QString &file)
{
    QFile dFile(file);
    QString FileName = file.split('/').last().split('.').first();
    if(dFile.open(QIODevice::ReadOnly))
    {
        //初始控件
        UWidgetScene * curWidget = nullptr;
        if(m_View->tabText(m_View->currentIndex()).contains("NEW"))
        {//当前待保存脚本为新建脚本
            curWidget =  qobject_cast<UWidgetScene*>(m_View->currentWidget());
        }

        //创建画面
        if(!curWidget)
        {
            curWidget = new UWidgetScene(UTabWidget::Scene,FileName,m_View);
            curWidget->loadSceneItem(dFile.readAll());
        }

        //添加画面
        m_View->addScene(FileName,curWidget);

        //模式
        if(m_isEditMode)
        {
            //切换到新增的页面
            m_View->setCurrentIndex(m_View->count()-1);

            //清除变化标识
            ClearChangeFlag(curWidget);

            //更新当前操作文档
            curWidget->setProperty("file",file);
        }

        return true;
    }
    return false;
}

void MainWindow::SetChangeFlag(QWidget *widget)
{
    int index = m_View->indexOf(widget);
    if(-1 != index )
    {
        if(!m_View->tabText(index).contains('*'))
            m_View->setTabText(index,m_View->tabText(index)+"*");
    }
}

void MainWindow::ClearChangeFlag(QWidget *widget)
{
    int index = m_View->indexOf(widget);
    if(-1 != index )
    {
        if(m_View->tabText(index).contains('*'))
            m_View->setTabText(index,m_View->tabText(index).remove('*'));
    }
}

bool MainWindow::GetChangeFlag(QWidget *widget)
{
    int index = m_View->indexOf(widget);
    if(-1 != index )
    {
        if(!m_View->tabText(index).contains('*'))
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
    for(int i = 0; i < m_View->count(); ++i)
    {
        if(GetChangeFlag(m_View->widget(i)))
        {
            bHaveNoSaveFile = true;
            break;
        }
    }
    if(bHaveNoSaveFile)
    {
        if(QMessageBox::Yes == QMessageBox::question(this,QString::fromLocal8Bit("提示"),
                                                     QString::fromLocal8Bit("当前还存在未保存的内容，是否保存后退出？")))
        {//确认关闭
            e->accept();
            saveAllSlot();
            //退出
            if(!onClose())
            {
              e->ignore();
            }
        }
        else
        {//不关闭
            e->ignore();
        }
    }
    else
    {
        //退出
        if(!onClose())
        {
          e->ignore();
        }
    }
}

bool MainWindow::onClose()
{
    int ret = QMessageBox::question(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("是否确认退出？"));
    bool bClose = false;
    switch (ret)
    {
    case QMessageBox::Yes:
    {
        bClose = true;
        //保存会话
        SaveCurrentSession();
        qApp->exit(0);
    }
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }
    return bClose;
}

void MainWindow::ShowMenu()
{
    if(m_toolBar)
    {
        m_toolBar->ShowMenu();
    }
}

void MainWindow::LoadCombineItems(const QString &sList)
{
    if(m_combineItemsToolbar)
    {
        m_combineItemsToolbar->clear();
        QStringList itemList = sList.split('#');
        QAction *itemAc = nullptr;
        for (const auto &item : itemList)
        {
            if(!item.isEmpty())
            {
                QString pngPath = QString("./CustomComponents/%1.png").arg(item);
                if(!QFile::exists(pngPath))
                {
                    continue;
                }
                itemAc = new QAction(this);
                itemAc->setIcon(QIcon(pngPath));
                itemAc->setText(item);
                connect(itemAc,SIGNAL(triggered()),this,SLOT(dealCombineItemSlot()));
                m_combineItemsToolbar->addAction(itemAc);
                m_combineItemsToolbar->addSeparator();
            }
        }
    }
}

void MainWindow::CustomItemselectDeal(const QString &item)
{
    if(m_View)
    {
        UWidgetScene * curSence = m_View->curScene();
        if(curSence)
        {
            curSence ->setCreateMode(item);
        }
    }
}




