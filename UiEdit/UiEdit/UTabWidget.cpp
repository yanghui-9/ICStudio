#include "UTabWidget.h"
#include "UWidgetScene.h"
#include "UiEdit.h"
#include <QScrollBar>
#include <QDebug>
#include <QTabBar>
#include <QDir>
#include "MainWindow.h"
#include "Itemsfactory.h"

UTabWidget::UTabWidget(QWidget *parent)
    :QTabWidget(parent)
{
    m_mainW = qobject_cast<MainWindow*>( parent );
    if(!m_mainW)
    {
        m_mainW = qobject_cast<MainWindow*>( parent->parent() );
    }
    //setMovable(true);
    //setTabBarAutoHide(true);//少于2个tab时自动隐藏
    //setTabShape(QTabWidget::Triangular);
    //setTabsClosable(true);


    if(m_mainW->GetModeFalg())
    {//编辑模式
        //右键菜单
        setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenuRequestedSlot(QPoint)));

        //接收鼠标移动事件，不管是否按下
        setMouseTracking(true);

        //管理工具栏，控件按下操作
        itemsFactory::GetInstance()->SetView(this);

        //tab可关闭的
        setTabsClosable(true);
    }
    else
    {
        //读取画面列表
        QDir dir("./project/scene");
        QFileInfoList fileList=dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::Hidden|QDir::NoDotAndDotDot,QDir::Name);
        for(int i = 0; i < fileList.size();i++)
        {
            if(fileList.at(i).suffix() == "scene")
            {
                m_sSceneList.append(fileList.at(i).fileName().split('.').first());
            }
        }
    }
}

UTabWidget::~UTabWidget()
{
    for(int i = 0; i < count(); ++i)
    {
        QWidget * w =  widget(i);
        removeTab(i--);
        delete w;
    }
    itemsFactory::ReleaseInstace();
}

void UTabWidget::mousePressEvent(QMouseEvent *event)
{
    QTabWidget::mousePressEvent(event);
}

void UTabWidget::mouseMoveEvent(QMouseEvent *event)
{
    QTabWidget::mouseMoveEvent(event);
}

void UTabWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QTabWidget::mouseReleaseEvent(event);
}

int UTabWidget::addScene(QString name, UWidgetScene *scene, bool isScene)
{
    if(Window == scene->getType() && !m_mainW->GetModeFalg() && !isScene)
    {//运行模式下的窗口处理
        if(!m_windowTab)
        {
            m_windowTab = new UTabWidget(this);
            m_windowTab->setHidden(true);
            m_windowTab->setTabBarAutoHide(true);
        }
        m_windowTab->addScene(name,scene,true);
    }
    else
    {
        scene->setVisible(true);
        scene->setGeometry(0,0,1024,600);
        scene->setMainStatusBar(m_mainW->m_statusBar);
        m_sceneList.insert(name,scene);
        addTab(scene,name);
    }
    return 0;
}

int UTabWidget::deleteScene(const QString &name)
{
    QMap<QString,UWidgetScene*>::Iterator it =  m_sceneList.find(name);
    if(m_sceneList.end() != it)
    {
        removeTab(indexOf(it.value()));
        m_sceneList.remove(name);
        return 0;
    }
    return -1;
}

int UTabWidget::deleteScene(UWidgetScene *scene)
{
    return deleteScene(m_sceneList.key(scene));
}

UWidgetScene *UTabWidget::getSceneFromName(const QString &name)
{
    UWidgetScene * sRet = nullptr;
    sRet = m_sceneList.value(name,nullptr);
    if(!sRet && m_windowTab)
    {
        sRet = m_windowTab->getSceneFromName(name);
    }
    //判断是否画面文件是否加载
    if(!sRet && -1 != m_sSceneList.indexOf(name))
    {
        m_mainW->OpenFromFile(QApplication::applicationDirPath()+WORKSPACE+"/"+ name +".scene");
        sRet = dynamic_cast<UWidgetScene*>( m_sceneList.value(name,nullptr) );
    }
    return sRet;
}

int UTabWidget::setCurScene(const QString &name)
{
    UWidgetScene* preS = getSceneFromName(m_curSceneID);
    UWidgetScene* curS = getSceneFromName(name);
    if(name == m_curSceneID)
    {//当前画面直接退出
        return 0;
    }
    if(curS)
    {//切换的画面存在
        if(UTabWidget::Window != curS->getType() ||
                (UTabWidget::Window == curS->getType() && m_mainW->GetModeFalg()))
        {//画面切换
            //先判断当前是否是窗口
            if(m_windowTab  && !m_windowID.isEmpty() )
            {//先处理窗口的关闭
                m_windowTab->setParent(this);
                m_windowTab->setHidden(true);
                m_windowID = "";
            }
            setCurrentWidget(curS);
            //编辑模式设置
            if(m_mainW->GetModeFalg())
            {
                if(!RectCreateUIInput)
                {//懒创建
                    RectCreateUIInput = new RectCreate_UIInput();
                    RectEditUIInput = new RectEdit_UIInput();
                    selectUIInput = new Select_UIInput();
                }
                RectCreateUIInput->setScreen(curS);
                RectEditUIInput->setScreen(curS);
                selectUIInput->setScreen(curS);
                curS->setSelectMode();
            }
            //切换画面时先切换后续数据和地址信息再刷新
            m_curSceneID = name;
            if(preS)
            {
                preS->dealScenceClose();
            }
            curS->dealScenceOpen();
        }
        else
        {//弹出窗口
            if( m_windowTab && preS )
            {
                m_windowTab->setCurScene(curS);
                preS->ShowWindow(m_windowTab,curS->getSceneRect());
                m_curSceneID  = name;
                m_windowID = name;
            }
        }
    }
    else
    {//要切换的窗口或者画面不存在，检查当前是否窗口，关闭窗口
        if(!m_windowID.isEmpty()  && UTabWidget::Window == preS->getType())
        {
            if(m_windowTab)
            {
                m_windowTab->setParent(this);
                m_windowTab->setHidden(true);
            }
        }
    }
    return 0;
}

void UTabWidget::setCurScene(UWidgetScene *scene)
{
    if(scene->getSceneName() == m_curSceneID)
    {//当前画面直接退出
        return ;
    }
    setCurrentWidget(scene);
    //切换画面时先切换后续数据和地址信息再刷新
    m_curSceneID = scene->getSceneName();
}

QStringList UTabWidget::compile()
{
    QStringList ret;
    QList <UWidgetScene*>  sceneList = m_sceneList.values();
    for(int i = 0; i < sceneList.size();i++)
    {
        UWidgetScene * sceneTmp = qobject_cast<UWidgetScene *>(sceneList.at(i));
        if(sceneTmp)
        {
            QStringList errorList =  sceneTmp->compile();
            if(0 != errorList.size())
            {
                ret = errorList;
                break;
            }
        }
    }
    return ret;
}

void UTabWidget::dealCompileError(QStringList &errInfoList)
{
    (void)errInfoList;
}

void UTabWidget::SetChangeFlag(QWidget *w)
{
    if(m_mainW)
    {
        m_mainW->SetChangeFlag(w);
    }
}

void UTabWidget::customContextMenuRequestedSlot(QPoint pos)
{
    (void)pos;
    if(m_mainW)
    {
        m_mainW->ShowMenu();
    }
}

UWidgetScene *UTabWidget::curScene()
{
    return dynamic_cast<UWidgetScene *>( currentWidget() );
}


int UTabWidget::initScene(const QString &name)
{
    if(m_sceneList.contains(name))
    {
        //((UWidgetScene*)m_sceneList.value(name))->loadSceneItem();
    }
    return 0;
}

