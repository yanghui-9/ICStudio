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

int UTabWidget::addScene(QString name, UWidgetScene *scene)
{
    scene->setVisible(true);
    scene->setGeometry(0,0,1024,600);
    scene->setMainStatusBar(m_mainW->m_statusBar);
    m_sceneList.insert(name,scene);
    addTab(scene,name);
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

int UTabWidget::setCurScene(const QString &name)
{
    UWidgetScene* preS = dynamic_cast<UWidgetScene*>( m_sceneList.value(m_curSceneID,nullptr) );
    UWidgetScene* curS = dynamic_cast<UWidgetScene*>( m_sceneList.value(name,nullptr) );
    if(!curS && -1 != m_sSceneList.indexOf(name))
    {//如果待切换的画面未加载
        m_mainW->OpenFromFile(QApplication::applicationDirPath()+WORKSPACE+"/"+ name +".scene");
        curS = dynamic_cast<UWidgetScene*>( m_sceneList.value(name,nullptr) );
    }
    if(name == m_curSceneID)//当前画面直接退出
    {
        return 0;
    }
    if(curS)
    {//切换的画面存在
        setCurrentWidget(curS);
        //setSceneRect(0,0,1024,600);
        //curS->setSceneRect(0,0,1024,600);

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
    return 0;
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

