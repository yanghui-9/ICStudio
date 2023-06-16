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
    m_fileToolBar = new QToolBar(this);
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

    addToolBar(m_fileToolBar);

    //初始化图库和变量表和脚本库，工具栏
    QToolBar *picToolbar = new QToolBar(this);
    QAction * picAC = new QAction(this);
    //picAC->setIcon(QIcon(":/image/gallery.png"));
    picAC->setText(QString::fromLocal8Bit("图库"));
    connect(picAC,&QAction::triggered,[this](){
       if(m_gallery) m_gallery->ShowImgSelectDialog(this);
       //图库编辑之后，控件对应的图片可能发生变化，刷新当前画面
       m_View->curScene()->reflashAllItem();
    });
    picToolbar->addAction(picAC);

    //变量表
    QAction * varC = new QAction(this);
    //varC->setIcon(QIcon(":/image/variables.png"));
    varC->setText(QString::fromLocal8Bit("变量表"));
    connect(varC,&QAction::triggered,[this](){
       if(m_varTable) m_varTable->ShowVarSelectDialog(this);
    });
    picToolbar->addAction(varC);

    //脚本
    QAction * scriptC = new QAction(this);
    //scriptC->setIcon(QIcon(":/image/script.png"));
    scriptC->setText(QString::fromLocal8Bit("脚本"));
    connect(scriptC,&QAction::triggered,[](){
        //调用外部脚本编辑器
        QProcess::execute("ScriptEditor.exe");
    });
    picToolbar->addAction(scriptC);

    //自定义组件
    QAction * combineC = new QAction(this);
    //combineC->setIcon(QIcon(":/image/script.png"));
    combineC->setText(QString::fromLocal8Bit("自定义组件"));
    connect(combineC,&QAction::triggered,[this](){
        if(!m_CustomComponentsDlg)
        {
            m_CustomComponentsDlg = new CustomComponentsDlg(this);
            m_CustomComponentsDlg->InitUI();
        }
        m_CustomComponentsDlg->SetSelectItems(m_customSelectItems);
        if(QDialog::Accepted == m_CustomComponentsDlg->exec())
        {
            m_customSelectItems = m_CustomComponentsDlg->GetSelectPic();
            this->LoadCombineItems(m_customSelectItems);
        }
    });
    picToolbar->addAction(combineC);

    //系统设置
    QAction * sysC = new QAction(this);
    //sysC->setIcon(QIcon(":/image/system.png"));
    sysC->setText(QString::fromLocal8Bit("系统设置"));
    connect(sysC,&QAction::triggered,[](){
        //
    });
    picToolbar->addAction(sysC);

    addToolBar(picToolbar);


    //初始化控件工具栏
    addToolBarBreak();
    m_toolBar = new UToolbar(this);
    m_toolBar->Init();

    //初始化自定义组件工具栏
    addToolBarBreak();
    m_combineItemsToolbar = new QToolBar("combines",this);
    addToolBar(Qt::RightToolBarArea,m_combineItemsToolbar);
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
        if(m_View)
        {
            UWidgetScene * curSence = m_View->curScene();
            if(curSence)
            {
                curSence ->setCreateMode(ac->text());
            }
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




