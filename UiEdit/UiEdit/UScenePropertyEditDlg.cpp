#include "UScenePropertyEditDlg.h"
#include <QDir>
#include "UWidgetScene.h"
#include "Gallery/interface_gallery.h"

UScenePropertyEditDlg::UScenePropertyEditDlg(UWidgetScene *scene, QWidget *parent):QDialog (parent),m_scene(scene)
{
    setWindowTitle(QString::fromLocal8Bit("画面属性编辑"));
    setMinimumSize(200,400);

    //读取脚本列表
    QDir dir("./project/script");
    QFileInfoList fileList=dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::Hidden|QDir::NoDotAndDotDot,QDir::Name);
    for(int i = 0; i < fileList.size();i++)
    {
        if(fileList.at(i).suffix() == "script")
        {
            m_scriptList.append(fileList.at(i).fileName().split('.').first());
        }
    }
}

void UScenePropertyEditDlg::InitUI()
{
    QVBoxLayout * mainLayout = new QVBoxLayout(this);

    //背景
    QGroupBox * m_background = new QGroupBox(QString::fromLocal8Bit("背景"),this);
    QGridLayout * bgLayout = new QGridLayout();
    colorRadio = new QRadioButton(QString::fromLocal8Bit("纯色"),this);
    connect(colorRadio,SIGNAL(clicked()),this,SLOT(bgTypeChangeSlot()));
    colorBtn = new QPushButton(this);
    connect(colorBtn,SIGNAL(clicked()),this,SLOT(colorSelectSlot()));
    picRadio = new QRadioButton(QString::fromLocal8Bit("图片"),this);
    connect(picRadio,SIGNAL(clicked()),this,SLOT(bgTypeChangeSlot()));
    picBtn = new QPushButton(QString::fromLocal8Bit("图片选择"),this);
    connect(picBtn,SIGNAL(clicked()),this,SLOT(imgSelectSlot()));
    bgLayout->addWidget(colorRadio,0,0,1,1);
    bgLayout->addWidget(colorBtn,0,1,1,1);
    bgLayout->addWidget(picRadio,1,0,1,1);
    bgLayout->addWidget(picBtn,1,1,1,1);
    m_background->setLayout(bgLayout);
    mainLayout->addWidget(m_background);

    //弹出窗口
    m_window = new QGroupBox(QString::fromLocal8Bit("弹出窗口"),this);
    m_window->setCheckable(true);
    m_window->setChecked(false);
    QLabel * X = new QLabel("X:",this);
    QLabel * Y = new QLabel("Y:",this);
    QLabel * W = new QLabel("W:",this);
    QLabel * H = new QLabel("H:",this);
    lineX = new QLineEdit(this);
    lineY = new QLineEdit(this);
    lineW = new QLineEdit(this);
    lineH = new QLineEdit(this);
    QGridLayout * winLayout = new QGridLayout();
    winLayout->addWidget(X,0,0,1,1);
    winLayout->addWidget(Y,1,0,1,1);
    winLayout->addWidget(W,2,0,1,1);
    winLayout->addWidget(H,3,0,1,1);
    winLayout->addWidget(lineX,0,1,1,1);
    winLayout->addWidget(lineY,1,1,1,1);
    winLayout->addWidget(lineW,2,1,1,1);
    winLayout->addWidget(lineH,3,1,1,1);
    m_window->setLayout(winLayout);
    //m_window->setEnabled(false);
    mainLayout->addWidget(m_window);

    //脚本
    m_scrit = new QGroupBox(QString::fromLocal8Bit("脚本"),this);
    m_scrit->setCheckable(true);
    m_scrit->setChecked(false);
    QGridLayout * scriptLayout = new QGridLayout();
    pressScript = new QCheckBox(QString::fromLocal8Bit("进入脚本："),this);
    pressCombox = new QComboBox(this);
    pressCombox->setEnabled(false);
    pressCombox->addItems(m_scriptList);
    connect(pressScript,SIGNAL(stateChanged(int)),this,SLOT(scriptCheckboxSlot(int)));
    releaseScript = new QCheckBox(QString::fromLocal8Bit("退出脚本:"),this);
    releaseCombox = new QComboBox(this);
    releaseCombox->setEnabled(false);
    releaseCombox->addItems(m_scriptList);
    connect(releaseScript,SIGNAL(stateChanged(int)),this,SLOT(scriptCheckboxSlot(int)));
    cycleScript = new QCheckBox(QString::fromLocal8Bit("循环脚本："),this);
    cycleCombox = new QComboBox(this);
    cycleCombox->setEnabled(false);
    cycTimeEdit = new QLineEdit(this);
    cycTimeEdit->setEnabled(false);
    cycleCombox->addItems(m_scriptList);
    connect(cycleScript,SIGNAL(stateChanged(int)),this,SLOT(scriptCheckboxSlot(int)));
    scriptLayout->addWidget(pressScript,0,0,1,1);
    scriptLayout->addWidget(pressCombox,0,1,1,2);
    scriptLayout->addWidget(releaseScript,1,0,1,1);
    scriptLayout->addWidget(releaseCombox,1,1,1,2);
    scriptLayout->addWidget(cycleScript,2,0,1,1);
    scriptLayout->addWidget(cycleCombox,2,1,1,1);
    scriptLayout->addWidget(cycTimeEdit,2,2,1,1);
    //设置比例
    scriptLayout->setColumnStretch(0,2);
    scriptLayout->setColumnStretch(1,2);
    scriptLayout->setColumnStretch(2,1);
    m_scrit->setLayout(scriptLayout);
    mainLayout->addWidget(m_scrit);

    //保存按钮
    QPushButton * saveBtn = new QPushButton(QString::fromLocal8Bit("保存"),this);
    connect(saveBtn,SIGNAL(clicked()),this,SLOT(saveSlot()));
    mainLayout->addWidget(saveBtn);

    setLayout(mainLayout);
}

void UScenePropertyEditDlg::RefreshUI(QJsonObject &sceneObj)
{
    m_sceneObj = sceneObj;
    //背景
    if(0 == sceneObj["background_type"].toInt(0))
    {
        colorRadio->setChecked(true);
        int color = sceneObj["background_color"].toInt(0xdcdcdc);
        //按钮设置背景色
        m_bgColor = QColor((color>>16)&0xff,(color>>8)&0xff,color&0xff);
        colorBtn->setStyleSheet(QString("background-color:#%1%2%3")
                                .arg((color>>16)&0xff,2,16,QChar('0'))
                                .arg((color>>8)&0xff,2,16,QChar('0'))
                                .arg(color&0xff,2,16,QChar('0')));
    }
    else
    {
        picRadio->setChecked(true);
        picBtn->setText(sceneObj["background_img"].toString());
    }
    //处理背景类型变化
    bgTypeChangeSlot();

    //窗口
    if(1 == sceneObj["window"].toInt(0))
    {
        m_window->setChecked(true);
        lineX->setText(QString::number( sceneObj["x"].toInt(0) ));
        lineY->setText(QString::number( sceneObj["y"].toInt(0) ));
        lineW->setText(QString::number( sceneObj["w"].toInt(0) ));
        lineH->setText(QString::number( sceneObj["h"].toInt(0) ));
    }
    else
    {
       m_window->setChecked(false);
    }

    //脚本
    if(1 == sceneObj["script"].toInt(0))
    {
        m_scrit->setChecked(true);
        if(1 == sceneObj["script_press"].toInt(0))
        {
            pressScript->setChecked(true);
            pressCombox->setCurrentText(sceneObj["script_press_name"].toString());
        }
        if(1 == sceneObj["script_release"].toInt(0))
        {
            releaseScript->setChecked(true);
            releaseCombox->setCurrentText(sceneObj["script_release_name"].toString());
        }
        if(1 == sceneObj["script_cycle"].toInt(0))
        {
            cycleScript->setChecked(true);
            cycleCombox->setCurrentText(sceneObj["script_cycle_name"].toString());
            cycTimeEdit->setText(QString::number( sceneObj["script_cycle_interval"].toInt(1000) ));
        }
    }
    else
    {
        m_scrit->setChecked(false);
    }
}

void UScenePropertyEditDlg::scriptCheckboxSlot(int state)
{
    QCheckBox * send = qobject_cast<QCheckBox *>(sender());
    if(send)
    {
        if(send == pressScript)
        {
            pressCombox->setEnabled(state);
        }
        else if (send == releaseScript) {
            releaseCombox->setEnabled(state);
        }
        else {
           cycleCombox->setEnabled(state);
        }
    }
}

void UScenePropertyEditDlg::saveSlot()
{
    //背景
    if(colorRadio->isChecked())
    {
       m_sceneObj["background_type"] = 0;
       m_sceneObj["background_color"] = (m_bgColor.red()<<16)+(m_bgColor.green()<<8)+m_bgColor.blue();
    }
    else
    {
       m_sceneObj["background_type"] = 1;
       m_sceneObj["background_img"] = picBtn->text();
    }
    //窗口
    if(m_window->isChecked())
    {
        m_sceneObj["window"] = 1;
        m_sceneObj["x"] = lineX->text().toInt();
        m_sceneObj["y"] = lineY->text().toInt();
        m_sceneObj["w"] = lineW->text().toInt();
        m_sceneObj["h"] = lineH->text().toInt();
    }
    else
    {
        m_sceneObj["window"] = int(0);
    }
    //脚本
    if(m_scrit->isChecked())
    {
        m_sceneObj["script"] = 1;
        if(pressScript->isChecked())
        {
           m_sceneObj["script_press"] = 1;
           m_sceneObj["script_press_name"] = pressCombox->currentText();
        }
        else
        {
           m_sceneObj["script_press"] = 0;
        }
        if(releaseScript->isChecked())
        {
           m_sceneObj["script_release"] = 1;
           m_sceneObj["script_release_name"] = releaseCombox->currentText();
        }
        else
        {
           m_sceneObj["script_release"] = 0;
        }
        if(cycleScript->isChecked())
        {
           m_sceneObj["script_cycle"] = 1;
           m_sceneObj["script_cycle_name"] = cycleCombox->currentText();
           m_sceneObj["script_cycle_interval"] = cycTimeEdit->text().toInt();
        }
        else
        {
           m_sceneObj["script_cycle"] = 0;
        }
    }
    accept();
}

void UScenePropertyEditDlg::colorSelectSlot()
{
    QColorDialog dlg(this);
    if(Accepted == dlg.exec())
    {
        QColor col = dlg.selectedColor();
        //按钮设置背景色
        m_bgColor = col;
        colorBtn->setStyleSheet(QString("background-color:#%1%2%3")
                                .arg(col.red(),2,16,QChar('0'))
                                .arg(col.green(),2,16,QChar('0'))
                                .arg(col.blue(),2,16,QChar('0')));
    }
}

void UScenePropertyEditDlg::imgSelectSlot()
{
    picBtn->setText(QString::fromLocal8Bit( m_scene->GetGallery()->ShowImgSelectDialog(this).data() ));
}

void UScenePropertyEditDlg::bgTypeChangeSlot()
{
    if(colorRadio->isChecked())
    {
        colorBtn->setEnabled(true);
        picBtn->setEnabled(false);
    }
    else
    {
        colorBtn->setEnabled(false);
        picBtn->setEnabled(true);
    }
}
