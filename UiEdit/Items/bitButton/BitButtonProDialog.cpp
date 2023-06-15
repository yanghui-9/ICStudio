#include "BitButtonProDialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QButtonGroup>
#include <QMessageBox>
#include "../interface_adapter.h"
#include "../../../basics_model/device_tools/include/tools.h"
#include "../../../extend_model/host_communication/linkParaInterface.h"
#include "../../../base_include/UAppBase.h"
#include "../../../base_include/interface_all_head.h"


BitButtonProDialog::BitButtonProDialog(const QString & name, QWidget *parent):
    QDialog(parent), m_name(name)
{
    this->setWindowTitle(QObject::tr(m_name.toUtf8().constData()));
    this->setMinimumSize(640, 360);

    //UHostApp * pApp = (UHostApp *)qApp;
    //m_prj = pApp->getCurrentProject();

    proValue0 = new PROVALUE;
    proValue1 = new PROVALUE;

    writeAddress_obj = NULL;
    monitorAddress_obj = NULL;

    m_bTextProChangeIn = false;

    initWidget();
}

BitButtonProDialog::~BitButtonProDialog()
{
    delete proValue0;
    delete proValue1;
    proValue0 = NULL;
    proValue1 = NULL;

    if (writeAddress_obj)
    {
        object_tree_destroy(writeAddress_obj);
    }
    if (monitorAddress_obj)
    {
        object_tree_destroy(monitorAddress_obj);
    }
}

void BitButtonProDialog::initWidget()
{  
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    QTabWidget *bar = new QTabWidget(this);

    QWidget *widget1 = new QWidget(bar);
    QWidget *widget2 = new QWidget(bar);

    bar->addTab(widget1,QObject::tr("General"));
    bar->addTab(widget2,QObject::tr("Label"));

    initGenaralTab(widget1);
    initLabelTab(widget2);

    if(m_name == "Bit Button")
    {
        // 高级页面
        m_seniorWidget = new SeniorSettingWidget(bar);
        /*【新增功能】位（字）触控功能，达到设定的状态值时控件被触发一下_maxiaofeng_20170227_Begin */
        m_seniorWidget->initWidget(1,false);
        /*【新增功能】位（字）触控功能，达到设定的状态值时控件被触发一下_maxiaofeng_20170227_End */
        bar->addTab(m_seniorWidget,QObject::tr("Advanced"));
    }

    // 显现页面
    m_visibilityWidget = new VisibilitySettingWidget(bar);
    m_visibilityWidget->initWidget(1);
    bar->addTab(m_visibilityWidget, QObject::tr("Visibility"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    m_confirmBtn = buttonBox->addButton(QObject::tr("Confirm"),QDialogButtonBox::AcceptRole);
    m_cancelBtn = buttonBox->addButton(QObject::tr("Cancel"),QDialogButtonBox::RejectRole);
    m_helpBtn = buttonBox->addButton(QObject::tr("Help"),QDialogButtonBox::HelpRole);

    connect(m_confirmBtn, SIGNAL(clicked()), this, SLOT(confirmSlot()));
    connect(m_cancelBtn, SIGNAL(clicked()), this, SLOT(close()));
    connect(m_helpBtn, SIGNAL(clicked()), this, SLOT(helpSlot()));

    mainLayout->addWidget(bar);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
}

void BitButtonProDialog::initGenaralTab(QWidget *w)
{
    QHBoxLayout * mainLayout = new QHBoxLayout(w);

    QWidget * preViewWidget = new QWidget(w);
    QVBoxLayout * preViewLayout = new QVBoxLayout(preViewWidget);

    QStringList labNames;
    labNames<<QObject::tr("Border Color:")
            <<QObject::tr("Foreground:")
            <<QObject::tr("Background:")
            <<QObject::tr("Pattern:");

    // 状态0预览
    QGroupBox * preViewGroupBox0 = new QGroupBox(QObject::tr("status0"), preViewWidget);
    QHBoxLayout * preViewLayout0 = new QHBoxLayout(preViewGroupBox0);

    QLabel * label0 = new QLabel(preViewGroupBox0);
    label0->setFixedSize(120,120);
    label0->move(0, 0);

    m_preView0 = new PreviewWidget(label0);
    m_preView0->setFixedSize(120,120);
    m_preView0->move(0,0);

    QWidget * statusWidget0 = new QWidget(preViewGroupBox0);
    QGridLayout * statusLayout0 = new QGridLayout(statusWidget0);

    for (int i = 0; i < 4; i++)
    {
        QLabel * label = new QLabel(labNames[i], statusWidget0);
        statusLayout0->addWidget(label, i, 0);
    }
    m_borderBtn0 = new ColorSelectPushButton(statusWidget0);
    m_foregroundBtn0 = new ColorSelectPushButton(statusWidget0);
    m_backgroundBtn0 = new ColorSelectPushButton(statusWidget0);
    m_brushStyleCombox0 = new BrushStyleComboBox(statusWidget0, true);
    statusLayout0->addWidget(m_borderBtn0, 0, 1);
    statusLayout0->addWidget(m_foregroundBtn0, 1, 1);
    statusLayout0->addWidget(m_backgroundBtn0, 2, 1);
    statusLayout0->addWidget(m_brushStyleCombox0, 3, 1);
    statusWidget0->setLayout(statusLayout0);

    preViewLayout0->addWidget(label0);
    preViewLayout0->addWidget(statusWidget0);
    preViewGroupBox0->setLayout(preViewLayout0);

    // 状态1预览
    QGroupBox * preViewGroupBox1 = new QGroupBox(QObject::tr("status1"), preViewWidget);
    QHBoxLayout * preViewLayout1 = new QHBoxLayout(preViewGroupBox1);

    QLabel * label1 = new QLabel(preViewGroupBox1);
    label1->setFixedSize(120,120);
    label1->move(0, 0);

    m_preView1 = new PreviewWidget(label1);
    m_preView1->setFixedSize(120,120);
    m_preView1->move(0,0);

    QWidget * statusWidget1 = new QWidget(preViewGroupBox1);
    QGridLayout * statusLayout1 = new QGridLayout(statusWidget1);

    for (int i = 0; i < 4; i++)
    {
        QLabel * label = new QLabel(labNames[i], statusWidget1);
        statusLayout1->addWidget(label, i, 0);
    }
    m_borderBtn1 = new ColorSelectPushButton(statusWidget1);
    m_foregroundBtn1 = new ColorSelectPushButton(statusWidget1);
    m_backgroundBtn1 = new ColorSelectPushButton(statusWidget1);
    m_brushStyleCombox1 = new BrushStyleComboBox(statusWidget1, true);
    statusLayout1->addWidget(m_borderBtn1, 0, 1);
    statusLayout1->addWidget(m_foregroundBtn1, 1, 1);
    statusLayout1->addWidget(m_backgroundBtn1, 2, 1);
    statusLayout1->addWidget(m_brushStyleCombox1, 3, 1);
    statusWidget1->setLayout(statusLayout1);

    preViewLayout1->addWidget(label1);
    preViewLayout1->addWidget(statusWidget1);
    preViewGroupBox1->setLayout(preViewLayout1);

    preViewLayout->addWidget(preViewGroupBox0);
    preViewLayout->addWidget(preViewGroupBox1);
    /*【新增功能】_文本控件和位指示灯添加闪烁功能_chiji_20190208_Begin*/
    if(m_name == "Bit Indicating Lamp")
    {
        QGroupBox *box3 = new QGroupBox(QObject::tr("Twinkle"), w);
        QGridLayout *gridLayout3 = new QGridLayout(box3);

        m_twinkleChebox = new QCheckBox(box3);
        m_autoChangeLabel = new QLabel(QObject::tr("Auto Change Frequency:"), box3);
        m_autoChangeSpinBox = new QSpinBox(box3);
        m_autoChangeSpinBox->setRange(1, 500);
        m_timeLabel = new QLabel(QObject::tr("*0.1s"), box3);

        connect(m_twinkleChebox,SIGNAL(stateChanged(int)),this,SLOT(flashCheSlot(int)));

        QLabel *testLabel = new QLabel(QObject::tr("                                       "), box3);
        gridLayout3->addWidget(m_twinkleChebox,0,0);
        gridLayout3->addWidget(m_autoChangeLabel,0,1);
        gridLayout3->addWidget(m_autoChangeSpinBox,0,3);
        gridLayout3->addWidget(m_timeLabel,0,4);
        gridLayout3->addWidget(testLabel,0,5);
        box3->setLayout(gridLayout3);
        preViewLayout->addWidget(box3);
    }
    /*【新增功能】_文本控件和位指示灯添加闪烁功能_chiji_20190208_End*/
    preViewWidget->setLayout(preViewLayout);

    // 功能和地址输入栏
    QWidget * addressWidget = new QWidget(w);
    QVBoxLayout * widgetLayout = new QVBoxLayout(addressWidget);
    m_addressWidget = new AddressWidget(addressWidget);
    m_addressWidget->setItemName(m_name);
    if (m_name == "Bit Button")
    {
        QGroupBox * functionGroupBox = new QGroupBox(QObject::tr("Functions"), addressWidget);
        /*功能增加，新增脚本功能，20180116，begin*/
        QGridLayout  * functionLayout = new QGridLayout(functionGroupBox);
        //QVBoxLayout * functionLayout = new QVBoxLayout(functionGroupBox);
        /*功能增加，新增脚本功能，20180116，end*/
        m_functionsGroup = new QButtonGroup(functionGroupBox);
        QStringList btnNames;
        btnNames<<QObject::tr("Set ON")
                <<QObject::tr("Set OFF")
                <<QObject::tr("Press down ON")
                <<QObject::tr("Press down OFF")
                <<QObject::tr("Invertal");
        for (int i = 0; i < 5; i++)
        {
            QRadioButton * button = new QRadioButton(btnNames[i], functionGroupBox);
            /*功能增加，新增脚本功能，20180116，begin*/
            //functionLayout->addWidget(button);
            functionLayout->addWidget(button,i,0,1,1);
            /*功能增加，新增脚本功能，20180116，end*/
            m_functionsGroup->addButton(button,i);
        }
        /*功能增加，新增脚本功能，20180116，begin*/
        m_macroFlag = new QRadioButton(QObject::tr("macro"),functionGroupBox);
        m_macroBox = new QComboBox(functionGroupBox);
        QStringList boxlist;
        boxlist<<QObject::tr("press macro")<<QObject::tr("release macro");
        m_macroBox->addItems(boxlist);
        m_macroEdit = new QPushButton(QObject::tr("edit macro"),functionGroupBox);
        connect(m_macroEdit,SIGNAL(clicked()),this,SLOT(macroEdit()));
        connect(m_macroFlag,SIGNAL(clicked(bool)),this,SLOT(macroClicked()));
        functionLayout->addWidget(m_macroFlag,0,1,1,1);
        functionLayout->addWidget(m_macroBox,1,1,1,1);
        functionLayout->addWidget(m_macroEdit,2,1,1,1);
        /*功能增加，新增脚本功能，20180116，end*/
        functionGroupBox->setLayout(functionLayout);

        m_addressWidget->initWidget(1);
        widgetLayout->addWidget(functionGroupBox);
        widgetLayout->addWidget(m_addressWidget);
        addressWidget->setLayout(widgetLayout);
    }
    else
    {
        m_addressWidget->initWidget(3);
        bitNumberWidget = new QWidget(addressWidget);
        QHBoxLayout * bitNumberHLayout = new QHBoxLayout(bitNumberWidget);
        QLabel * m_bitNumberLabel = new QLabel(QObject::tr("Bit Number"), bitNumberWidget);
        m_bitNumberComboBox = new QComboBox(bitNumberWidget);
        bitNumberHLayout->addWidget(m_bitNumberLabel);
        bitNumberHLayout->addWidget(m_bitNumberComboBox);
        bitNumberWidget->setLayout(bitNumberHLayout);
        connect(m_addressWidget->m_dataCombox1,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexSlot(int)));

        widgetLayout->addWidget(m_addressWidget);
        widgetLayout->addWidget(bitNumberWidget);
        addressWidget->setLayout(widgetLayout);
    }

    mainLayout->addWidget(preViewWidget);
    mainLayout->addWidget(addressWidget);
    w->setLayout(mainLayout);

    connect(m_borderBtn0, SIGNAL(colorValueChanged(QColor)), this, SLOT(colorValueChanged(QColor)));
    connect(m_foregroundBtn0, SIGNAL(colorValueChanged(QColor)), this, SLOT(colorValueChanged(QColor)));
    connect(m_backgroundBtn0, SIGNAL(colorValueChanged(QColor)), this, SLOT(colorValueChanged(QColor)));
    connect(m_borderBtn1, SIGNAL(colorValueChanged(QColor)), this, SLOT(colorValueChanged(QColor)));
    connect(m_foregroundBtn1, SIGNAL(colorValueChanged(QColor)), this, SLOT(colorValueChanged(QColor)));
    connect(m_backgroundBtn1, SIGNAL(colorValueChanged(QColor)), this, SLOT(colorValueChanged(QColor)));

    connect(m_brushStyleCombox0, SIGNAL(brushStyleValueChanged(int)), this, SLOT(brushStyleValueChanged(int)));
    connect(m_brushStyleCombox1, SIGNAL(brushStyleValueChanged(int)), this, SLOT(brushStyleValueChanged(int)));
}

void BitButtonProDialog::initLabelTab(QWidget *w)
{
    QHBoxLayout * mainLayout = new QHBoxLayout(w);

    QGroupBox * offGroupBox = new QGroupBox(QObject::tr("Status OFF"), w);
    QVBoxLayout * offLayout = new QVBoxLayout(offGroupBox);
    m_textWidget0 = new TextWidget(offGroupBox);
    m_imageWidget0 = new ImageWidget(offGroupBox);
    offLayout->addWidget(m_textWidget0);
    offLayout->addWidget(m_imageWidget0);
    offGroupBox->setLayout(offLayout);

    QGroupBox * onGroupBox = new QGroupBox(QObject::tr("Status ON"), w);
    QVBoxLayout * onLayout = new QVBoxLayout(onGroupBox);
    m_textWidget1 = new TextWidget(onGroupBox);
    m_imageWidget1 = new ImageWidget(onGroupBox);
    onLayout->addWidget(m_textWidget1);
    onLayout->addWidget(m_imageWidget1);
    onGroupBox->setLayout(onLayout);

    mainLayout->addWidget(offGroupBox);
    mainLayout->addWidget(onGroupBox);
    w->setLayout(mainLayout);

    connect(m_textWidget0,SIGNAL(textValuesChanged(QString,QVariant)),this,SLOT(textValuesChanged(QString,QVariant)));
    connect(m_textWidget1,SIGNAL(textValuesChanged(QString,QVariant)),this,SLOT(textValuesChanged(QString,QVariant)));
    connect(m_imageWidget0,SIGNAL(imageValuesChanged(QString,QVariant)),this,SLOT(imageValuesChanged(QString,QVariant)));
    connect(m_imageWidget1,SIGNAL(imageValuesChanged(QString,QVariant)),this,SLOT(imageValuesChanged(QString,QVariant)));
}

void BitButtonProDialog::refreshUIfromData(struct object *obj)
{
    m_bTextProChangeIn = true;

    m_object = obj;

    m_preView0->setItemObject(m_object);
    m_preView1->setItemObject(m_object);

    if (m_name == "Bit Button")
    {
        m_preView0->setItemPropertyChanged(m_preView0->getItemPropertyChanged("Cur-status"), QString("status0"));
        m_preView1->setItemPropertyChanged(m_preView1->getItemPropertyChanged("Cur-status"), QString("status1"));
    }
    else if (m_name == "Bit Indicating Lamp")
    {
        m_preView0->setItemPropertyChanged(m_preView0->getItemPropertyChanged("statusIndex"), 0);
        m_preView1->setItemPropertyChanged(m_preView1->getItemPropertyChanged("statusIndex"), 1);
        //yangjindong--------------------
        /*【新增功能】_文本控件和位指示灯添加闪烁功能_chiji_20190208_Begin*/
        struct property *twinklePro = object_get_property(obj,"flashLamp");
        int twinkleIndex = property_get_value_type_int32(twinklePro,0);

        struct property *flashTimePro = object_get_property(obj,"flashTime");
        int flashTime = property_get_value_type_int32(flashTimePro,10);
        //yangjindong--------------------
        m_autoChangeSpinBox->setValue(flashTime);

        if(twinkleIndex == 0)
        {
            m_twinkleChebox->setCheckState(Qt::Unchecked);
            m_autoChangeSpinBox->setEnabled(false);
        }
        else if(twinkleIndex == 2)
        {
             m_twinkleChebox->setCheckState(Qt::Checked);
             m_autoChangeSpinBox->setEnabled(true);
        }
        /*【新增功能】_文本控件和位指示灯添加闪烁功能_chiji_20190208_End*/
    }

    if(obj)
    {
        struct property * status0_pro = object_get_property(obj, "status0");
        struct property * status1_pro = object_get_property(obj, "status1");
        getPropertyValue(status0_pro,proValue0);
        getPropertyValue(status1_pro,proValue1);

        m_borderBtn0->m_color = proValue0->m_borderColor;
        m_borderBtn0->refreshButtonBkColor();
        m_backgroundBtn0->m_color = proValue0->m_backgroundColor;
        m_backgroundBtn0->refreshButtonBkColor();
        m_foregroundBtn0->m_color = proValue0->m_foregroundColor;
        m_foregroundBtn0->refreshButtonBkColor();
        m_brushStyleCombox0->setCurrentIndex(proValue0->m_brushStyle);

        m_borderBtn1->m_color = proValue1->m_borderColor;
        m_borderBtn1->refreshButtonBkColor();
        m_backgroundBtn1->m_color = proValue1->m_backgroundColor;
        m_backgroundBtn1->refreshButtonBkColor();
        m_foregroundBtn1->m_color = proValue1->m_foregroundColor;
        m_foregroundBtn1->refreshButtonBkColor();
        m_brushStyleCombox1->setCurrentIndex(proValue1->m_brushStyle);

        m_textWidget0->setFontColor(proValue0->m_textColor);
        m_textWidget0->setFontFamliyIndex(proValue0->m_fontFamily);
        m_textWidget0->setFontSize(proValue0->m_fontSize);
        m_textWidget0->setFontBold(proValue0->m_fontBold);
        m_textWidget0->setFontItalic(proValue0->m_fontItalic);
        m_textWidget0->setFontUnderLine(proValue0->m_fontUnderLine);
        m_textWidget0->setFontKerning(proValue0->m_letterSpacing);
        m_textWidget0->setTextPosition(proValue0->m_textPositon);
        m_textWidget0->setTextEditText(proValue0->m_text);
        m_imageWidget0->setDisplayImage(proValue0->m_image);

        m_textWidget1->setFontColor(proValue1->m_textColor);
        m_textWidget1->setFontFamliyIndex(proValue1->m_fontFamily);
        m_textWidget1->setFontSize(proValue1->m_fontSize);
        m_textWidget1->setFontBold(proValue1->m_fontBold);
        m_textWidget1->setFontItalic(proValue1->m_fontItalic);
        m_textWidget1->setFontUnderLine(proValue1->m_fontUnderLine);
        m_textWidget1->setFontKerning(proValue1->m_letterSpacing);
        m_textWidget1->setTextPosition(proValue1->m_textPositon);
        m_textWidget1->setTextEditText(proValue1->m_text);
        m_imageWidget1->setDisplayImage(proValue1->m_image);

        monitorAddress_obj = object_tree_clone(object_get_child(m_object,"Maddress"));
    }

    if(m_name =="Bit Button")
    {
        //yangjindong--------------------
        // 功能
        functions_pro = object_get_property(m_object, "functions");
        m_functionsGroup->button(property_get_value_type_int32(functions_pro,4))->setChecked(true);

        monitor_pro = object_get_property(m_object,"monitor");
        sameaddress_pro = object_get_property(m_object,"sameaddress");

        m_addressWidget->setMonitorChecked(property_get_value_type_int32(monitor_pro,1));
        m_addressWidget->setWMaddressChecked(property_get_value_type_int32(sameaddress_pro,1));
        //yangjindong--------------------

        writeAddress_obj = object_tree_clone(object_get_child(m_object,"Waddress"));

        m_addressWidget->setAddressData(writeAddress_obj,0,1);      // 写入地址
        m_addressWidget->setAddressData(monitorAddress_obj,0,1);    // 监视地址
        /*功能增加，新增脚本功能，20180116，begin*/
//        if(!object_get_property(obj,"macroFlag"))
//        {
//            object_create_property_number(obj,"macroFlag",0);
//        }
//        if(!object_get_property(obj,"pressmacro"))
//        {
//            object_create_property_ustring(obj,"pressmacro","");
//        }
//        if(!object_get_property(obj,"releasemacro"))
//        {
//            object_create_property_ustring(obj,"releasemacro","");
//        }
//        if(!object_get_property(obj,"macroPressFontSize"))
//        {
//            object_create_property_uint32(obj,"macroPressFontSize",6);
//        }
//        if(!object_get_property(obj,"macroPressFontStr"))
//        {
//            object_create_property_ustring(obj,"macroPressFontStr",QString("仿宋").toUtf8().constData());
//        }
//        if(!object_get_property(obj,"macroReleaseFontSize"))
//        {
//            object_create_property_uint32(obj,"macroReleaseFontSize",6);
//        }
//        if(!object_get_property(obj,"macroReleaseFontStr"))
//        {
//            object_create_property_ustring(obj,"macroReleaseFontStr",QString("仿宋").toUtf8().constData());
//        }
//        if(!object_get_property(obj,"macroStyle"))
//        {
//            object_create_property_uint32(obj,"macroStyle",0);
//        }
        m_macroBox->setCurrentIndex(object_get_property_value_type_int32(obj,"macroStyle",0));
        m_macroFlag->setChecked(object_get_property_value_type_int32(obj,"macroFlag",0));
        macroClicked();
        m_pressMacro = object_get_property_value_type_string(obj,"pressmacro","");
        m_releaseMacro = object_get_property_value_type_string(obj,"releasemacro","");
        m_macroPressFontStr = object_get_property_value_type_string(obj,"macroPressFontStr",QString::fromUtf8("Arial"));
        m_macroPressFontSize = object_get_property_value_type_uint32(obj,"macroPressFontSize",8);
        m_macroReleaseFontStr = object_get_property_value_type_string(obj,"macroReleaseFontStr",QString::fromUtf8("Arial"));
        m_macroReleaseFontSize = object_get_property_value_type_uint32(obj,"macroReleaseFontSize",8);
        /*功能增加，新增脚本功能，20180116，end*/
    }
    else
    {
        // 数据类型
        //yangjindong--------------------
        dataType_pro = object_get_property(obj, "data type");
        m_addressWidget->m_dataCombox1->setCurrentIndex(property_get_value_type_int32(dataType_pro,0));

        QStringList datas;
        datas<<QObject::tr("Bit")
            <<QObject::tr("Word bits")
            <<QObject::tr("Double-word bits");

        m_addressWidget->setSelectDataType(datas,property_get_value_type_int32(dataType_pro));
        if (m_addressWidget->m_dataCombox1->currentIndex() == 0) // 位
            m_addressWidget->setAddressData(monitorAddress_obj,0,1);
        else if (m_addressWidget->m_dataCombox1->currentIndex() == 1) // 字的位
            m_addressWidget->setAddressData(monitorAddress_obj, 1, 1);
        else // 双字的位
            m_addressWidget->setAddressData(monitorAddress_obj, 1, 2);

        // 位编号
        bitNumber_pro = object_get_property(obj, "bit number");
        m_bitNumberComboBox->setCurrentIndex(property_get_value_type_int32(bitNumber_pro,0));
        //yangjindong--------------------
    }

    m_bTextProChangeIn = false;
}

void BitButtonProDialog::getPropertyValue(struct property *pro, PROVALUE *value)
{
    //yangjindong--------------------
    // 边框颜色
    struct property * border = property_get_subproperty(pro, "border color");
    if(border)
        value->m_borderColor =  property_get_value_type_color(border);
    else
        value->m_borderColor =  QColor(255,255,255,255);
    // 背景颜色
    QColor initbackgroundColor;
    if (m_name == "Bit Button")
        initbackgroundColor = QColor(238,238,238,255);
    else
        initbackgroundColor = QColor(192,192,192,255);
    struct property * background = property_get_subproperty(pro, "background color");
    if(background)
        value->m_backgroundColor = property_get_value_type_color(background);
    else
        value->m_backgroundColor = initbackgroundColor;
    // 背前景颜色
    struct property * foreground = property_get_subproperty(pro, "foreground color");
    if(foreground)
        value->m_foregroundColor = property_get_value_type_color(foreground);
    else
        value->m_foregroundColor = QColor(0, 0, 0, 255);
    // 填充类型
    struct property * pattern = property_get_subproperty(pro, "pattern");
    if(pattern)
        value->m_brushStyle = property_get_value_type_int32(pattern);
    else
        value->m_brushStyle =1;
    int lcount = 0;
    int languageID = tools_getSystemCurrentLanguage();//interface_adapter::getInstance()->getSystemCurrentLanguage();//m_prj->getSystemCurrentLanguage(lcount);
    QByteArray ba = QString("Language"+QString::number(languageID)).toUtf8();
    const char *tName = ba.constData();
    struct property * textPro = property_get_subproperty(pro,"text");
    struct property * lanPro = property_get_subproperty(textPro,tName);
    struct property * font = property_get_subproperty(lanPro, "font");
    text_pro = textPro;
    lan_pro = lanPro;
    struct property *font_family = property_get_subproperty(font, "font_family");
    struct property *font_size = property_get_subproperty(font, "font_pixelSize");
    struct property *font_bold = property_get_subproperty(font, "font_bold");
    struct property *font_italic = property_get_subproperty(font, "font_italic");
    struct property *font_underLine = property_get_subproperty(font, "font_underline");
    struct property *font_spacing = property_get_subproperty(font, "font_letterSpacing");
    struct property *fontColor_pro = property_get_subproperty(font, "font_color");
    struct property *position_pro = property_get_subproperty(font, "text_position");

    //更新一次全局字体
     getGlobalFont();
    //字体
    int familyIndex = fontDatabase.families().indexOf(property_get_value_type_string(font_family));
    /*【功能优化】默认字体添加Arial备选，其处理电脑不存在该字体造成的崩溃问题_yangjindong20201130_begin*/
    if(familyIndex<0)
    {
        if(fontDatabase.families().contains("Arial"))
            familyIndex = fontDatabase.families().indexOf("Arial");
        else
            familyIndex = 0;
    }
    if(fontDatabase.families().indexOf(g_font_family)<0)
    {
        if(fontDatabase.families().contains("Arial"))
            g_font_family = "Arial";
        else
            g_font_family = fontDatabase.families().at(0);
    }
    /*【功能优化】默认字体添加Arial备选，其处理电脑不存在该字体造成的崩溃问题_yangjindong20201130_end*/
    if(font_family)
        value->m_fontFamily = familyIndex;
    else
        value->m_fontFamily = fontDatabase.families().indexOf(g_font_family);
    if(font_size)
        value->m_fontSize = property_get_value_type_int32(font_size);
    else
        value->m_fontSize =   g_font_pixelSize;
    if(font_bold)
        value->m_fontBold = property_get_value_type_int32(font_bold);
    else
        value->m_fontBold =  g_font_bold;
    if(font_italic)
        value->m_fontItalic = property_get_value_type_int32(font_italic);
    else
        value->m_fontItalic = g_font_italic;
    if(font_underLine)
        value->m_fontUnderLine = property_get_value_type_int32(font_underLine);
    else
        value->m_fontUnderLine =   g_font_underline;
    if(font_spacing)
        value->m_letterSpacing = property_get_value_type_int32(font_spacing);
    else
        value->m_letterSpacing = g_font_letterSpacing;
    if(fontColor_pro)
        value->m_textColor = property_get_value_type_color(fontColor_pro);
    else
        value->m_textColor =  g_font_color;
    if(position_pro)
        value->m_textPositon = property_get_value_type_int32(position_pro);
    else
        value->m_textPositon =   g_text_position;

    // 文本
    value->m_text = property_get_value_type_string(lanPro);

    // 图片
    value->m_image = property_get_value_type_string(property_get_subproperty(pro, "image"),"");
    //yangjindong--------------------

}


void BitButtonProDialog::confirmSlot()
{
    if (m_name == "Bit Button")
	{
        //　检查高级页面设置
        if(!m_seniorWidget->checkSeniorSettings())
            return;
	}

    // 检查显现页面设置
    if (!m_visibilityWidget->checkVisibilitySettings())
        return;
    //yangjindong--------------------
    struct property * status_pro0 = object_get_property(m_object, "status0");
    struct property * status_pro1 = object_get_property(m_object, "status1");
    /*【新增功能】_文本控件和位指示灯添加闪烁功能_chiji_20190208_Begin*/
    if(m_name == "Bit Indicating Lamp")
    {
        struct property *thePro = object_get_property(m_object,"flashTime");
        if(!thePro&&m_autoChangeSpinBox->value()!=10)
            thePro=object_create_property_number(m_object, "flashTime", 10);
        property_set_value_number(thePro, m_autoChangeSpinBox->value());
    }
    /*【新增功能】_文本控件和位指示灯添加闪烁功能_chiji_20190208_End*/
    if (m_name == "Bit Button")
    {
        if(m_addressWidget->getWriteAddress().isEmpty() )
        {
            QMessageBox::information(this, tr("Warning"), tr("Please input the address!"), tr("Ok"));
            return;
        }
        if(m_addressWidget->getMonitorCheck() && m_addressWidget->getMonitorAddress().isEmpty() )
        {
            QMessageBox::information(this, tr("Warning"), tr("Please input the monitor address!"), tr("Ok"));
            return;
        }

        QString WAddr = m_addressWidget->getWriteAddress();
        int ret = tools_checkAddress(writeAddress_obj, WAddr, TYPE_BIT, 1, RWFLAG_WRITE);//interface_adapter::getInstance()->checkAddress(writeAddress_obj, WAddr, TYPE_BIT, 1, RWFLAG_WRITE);
        if (ret < 0)
            return;

        if(m_addressWidget->getMonitorCheck())
        {
            QString MAddr = m_addressWidget->getMonitorAddress();
            int ret = tools_checkAddress(monitorAddress_obj, MAddr, TYPE_BIT, 1, RWFLAG_READ);//interface_adapter::getInstance()->checkAddress(monitorAddress_obj, MAddr, TYPE_BIT, 1, RWFLAG_READ);
            if (ret < 0)
                return;
        }
        /*功能增加，新增脚本功能，20180116，begin*/
        struct property * macroflagPro = object_get_property(m_object,"macroFlag");
        struct property * pressmacroPro = object_get_property(m_object,"pressmacro");
        struct property * releasemacroPro = object_get_property(m_object,"releasemacro");
        struct property * pressFontSizePro = object_get_property(m_object,"macroPressFontSize");
        struct property * pressFontStrPro = object_get_property(m_object,"macroPressFontStr");
        struct property * releaseFontSizePro = object_get_property(m_object,"macroReleaseFontSize");
        struct property * releaseFontStrPro = object_get_property(m_object,"macroReleaseFontStr");
        struct property * macroStylePro = object_get_property(m_object,"macroStyle");
        if(!macroflagPro&&m_macroFlag->isChecked()!=0)
        {
            macroflagPro=object_create_property_number(m_object,"macroFlag",0);
        }
        if(!pressmacroPro&&m_pressMacro!="")
        {
            pressmacroPro=object_create_property_ustring(m_object,"pressmacro","");
        }
        if(!releasemacroPro&&m_releaseMacro!="")
        {
            releasemacroPro=object_create_property_ustring(m_object,"releasemacro","");
        }
        if(!pressFontSizePro&&m_macroPressFontSize!=6)
        {
            pressFontSizePro=object_create_property_uint32(m_object,"macroPressFontSize",6);
        }
        if(!pressFontStrPro&&m_macroPressFontStr!=QString::fromUtf8("仿宋"))
        {
            pressFontStrPro=object_create_property_ustring(m_object,"macroPressFontStr",QString("仿宋").toUtf8().constData());
        }
        if(!releaseFontSizePro&&m_macroReleaseFontSize!=6)
        {
            releaseFontSizePro=object_create_property_uint32(m_object,"macroReleaseFontSize",6);
        }
        if(!releaseFontStrPro&&m_macroReleaseFontStr!=QString::fromUtf8("仿宋"))
        {
            releaseFontStrPro=object_create_property_ustring(m_object,"macroReleaseFontStr",QString("仿宋").toUtf8().constData());
        }
        if(!macroStylePro&&m_macroBox->currentIndex()!=0)
        {
            macroStylePro=object_create_property_uint32(m_object,"macroStyle",0);
        }
        property_set_value_uint32(macroStylePro,m_macroBox->currentIndex());
        property_set_value_number(macroflagPro,m_macroFlag->isChecked());
        property_set_value_ustring(pressmacroPro,m_pressMacro.toUtf8().constData());
        property_set_value_ustring(releasemacroPro,m_releaseMacro.toUtf8().constData());
        property_set_value_ustring(pressFontStrPro,m_macroPressFontStr.toUtf8().constData());
        property_set_value_uint32(pressFontSizePro,m_macroPressFontSize);
        property_set_value_ustring(releaseFontStrPro,m_macroReleaseFontStr.toUtf8().constData());
        property_set_value_uint32(releaseFontSizePro,m_macroReleaseFontSize);
        /*功能增加，新增脚本功能，20180116，end*/
    }
    else
    {
        QString MAddr = m_addressWidget->getMonitorAddress();

        int ret = -1;
        if (m_addressWidget->m_dataCombox1->currentIndex() == 0)//位
        {
            ret = tools_checkAddress(monitorAddress_obj, MAddr, TYPE_BIT, 1, RWFLAG_READ);//interface_adapter::getInstance()->checkAddress(monitorAddress_obj, MAddr, TYPE_BIT, 1, RWFLAG_READ);
        }
        else if (m_addressWidget->m_dataCombox1->currentIndex() == 1)//字的位
        {
            ret = tools_checkAddress(monitorAddress_obj, MAddr, USHORT_16BIT, 1, RWFLAG_READ);//interface_adapter::getInstance()->checkAddress(monitorAddress_obj, MAddr, USHORT_16BIT, 1, RWFLAG_READ);
        }
        else
        {
            ret = tools_checkAddress(monitorAddress_obj, MAddr, UINT_32BIT, 2, RWFLAG_READ);
        }

        if (ret < 0)
            return;
    }

    // 1.先保存控件各状态的值
    proValue0->m_borderColor = m_borderBtn0->m_color;
    proValue0->m_backgroundColor = m_backgroundBtn0->m_color;
    proValue0->m_foregroundColor = m_foregroundBtn0->m_color;
    proValue0->m_brushStyle = m_brushStyleCombox0->currentIndex();

    proValue1->m_borderColor = m_borderBtn1->m_color;
    proValue1->m_backgroundColor = m_backgroundBtn1->m_color;
    proValue1->m_foregroundColor = m_foregroundBtn1->m_color;
    proValue1->m_brushStyle = m_brushStyleCombox1->currentIndex();

    // 2.保存属性值到object中
    savePropertyValue(proValue0,status_pro0,m_textWidget0,m_imageWidget0);
    savePropertyValue(proValue1,status_pro1,m_textWidget1,m_imageWidget1);

    if (m_name == "Bit Button")
    {
        // 功能
        if(!monitor_pro&&m_addressWidget->getMonitorCheck()!=1)
            monitor_pro = object_create_property_number(m_object, "monitor", 1);
        property_set_value_number(monitor_pro,m_addressWidget->getMonitorCheck());
        if(!sameaddress_pro&&m_addressWidget->getWMaddressCheck()!=1)
            sameaddress_pro=object_create_property_number(m_object, "sameaddress", 1);
        property_set_value_number(sameaddress_pro,m_addressWidget->getWMaddressCheck());

        if(!functions_pro&&m_functionsGroup->checkedId()!=4)
            functions_pro = object_create_property_number(m_object, "functions", 4);
        property_set_value_number(functions_pro, m_functionsGroup->checkedId());

        copyAddressByObj(writeAddress_obj, object_get_child(m_object,"Waddress"));// 保存写入地址
    }
    else
    {
        // 数据类型
        if(!dataType_pro&&m_addressWidget->m_dataCombox1->currentIndex()!=0)
            dataType_pro=object_create_property_number(m_object, "data type", 0);
        property_set_value_number(dataType_pro, m_addressWidget->m_dataCombox1->currentIndex());

        // 位编号
        if(!bitNumber_pro&&m_bitNumberComboBox->currentIndex()!=0)
            bitNumber_pro=object_create_property_number(m_object,"bit number",0);
        property_set_value_number(bitNumber_pro, m_bitNumberComboBox->currentIndex());
    }
    //yangjindong--------------------

    copyAddressByObj(monitorAddress_obj, object_get_child(m_object,"Maddress"));// 保存监视地址

    tools_projectChange();
    QDialog::accept();
}

void BitButtonProDialog::helpSlot()
{
    if (m_name == "Bit Button")
    {
        tools_callHelp(29);
    }
    else if (m_name == "Bit Indicating Lamp")
    {
        tools_callHelp(42);
    }
}

void BitButtonProDialog::currentIndexSlot(int index)
{
    QStringList bitNumberList;
    if (index == 0)
    {
        bitNumberWidget->hide();
    }
    else if (index == 1)
    {
        for (int i = 0; i < 16; i++)
        {
            bitNumberList<<QString("%1").arg(i);
        }
        bitNumberWidget->show();
    }
    else
    {
        for (int i = 0; i < 32; i++)
        {
            bitNumberList<<QString("%1").arg(i);
        }
        bitNumberWidget->show();
    }

    m_bitNumberComboBox->clear();
    m_bitNumberComboBox->addItems(bitNumberList);

    if (m_addressWidget->m_dataCombox1->currentIndex() == 0) // 位
    {
        m_addressWidget->m_bitOrWord = 0;
        m_addressWidget->m_addressSize = 1;
    }
    else if (m_addressWidget->m_dataCombox1->currentIndex() == 1) // 字的位
    {
        m_addressWidget->m_bitOrWord = 1;
        m_addressWidget->m_addressSize = 1;
    }
    else // 双字的位
    {
        m_addressWidget->m_bitOrWord = 1;
        m_addressWidget->m_addressSize = 2;
    }
}

void BitButtonProDialog::savePropertyValue(PROVALUE *proValue,struct property * status_pro,TextWidget * textWidget,ImageWidget * imageWidget)
{
    //yangjindong--------------------
    // 边框颜色
    borderColor_pro = property_get_subproperty(status_pro, "border color");
    if(!borderColor_pro&&proValue->m_borderColor!=QColor(255,255,255,255))
    {
        borderColor_pro = property_create_subproperty_color(status_pro, "border color", QColor(255,255,255,255));
    }
    property_set_value_color(borderColor_pro, proValue->m_borderColor);

    QColor initbackgroundColor;
    if (m_name == "Bit Button")
        initbackgroundColor = QColor(238,238,238,255);
    else
        initbackgroundColor = QColor(192,192,192,255);

    // 背景颜色
    backgroundColor_pro = property_get_subproperty(status_pro, "background color");
    if(!backgroundColor_pro&&proValue->m_backgroundColor!=initbackgroundColor)
    {
        backgroundColor_pro = property_create_subproperty_color(status_pro, "background color", initbackgroundColor);
    }
    property_set_value_color(backgroundColor_pro, proValue->m_backgroundColor);

    //前景色颜色
    foregroundColor_pro = property_get_subproperty(status_pro, "foreground color");
    if(!foregroundColor_pro&&proValue->m_foregroundColor!=QColor(0, 0, 0, 255))
    {
        foregroundColor_pro = property_create_subproperty_color(status_pro, "foreground color", QColor(0, 0, 0, 255));
    }
    property_set_value_color(foregroundColor_pro, proValue->m_foregroundColor);

    // 填充类型
    pattern_pro = property_get_subproperty(status_pro, "pattern");
    if(!pattern_pro&&proValue->m_brushStyle!=1)
    {
        pattern_pro = property_create_subproperty_number(status_pro, "pattern", 1);
    }
    property_set_value_number(pattern_pro, proValue->m_brushStyle);

    getGlobalFont();
    int languageID = tools_getSystemCurrentLanguage();//interface_adapter::getInstance()->getSystemCurrentLanguage();//m_prj->getSystemCurrentLanguage(lcount);
    QByteArray ba = QString("Language"+QString::number(languageID)).toUtf8();
    const char *tName = ba.constData();
    struct property * textPro = property_get_subproperty(status_pro,"text");
    struct property * lanPro = property_get_subproperty(textPro,tName);
    struct property * font = property_get_subproperty(lanPro, "font");
    struct property *font_family = property_get_subproperty(font, "font_family");
    if(!font_family&&fontDatabase.families().at(textWidget->getFontFamliyIndex())!=g_font_family)
    {
        font_family=property_create_subproperty_ustring(font, "font_family", g_font_family.toUtf8().constData());
    }
    property_set_value_ustring(font_family, fontDatabase.families().at(textWidget->getFontFamliyIndex()).toUtf8().constData());

    struct property *font_size = property_get_subproperty(font, "font_pixelSize");
    if(!font_size&&textWidget->getFontSize()!=g_font_pixelSize)
    {
        font_size=property_create_subproperty_number(font, "font_pixelSize", g_font_pixelSize);
    }
    property_set_value_number(font_size, textWidget->getFontSize());

    struct property *font_bold = property_get_subproperty(font, "font_bold");
    if(!font_bold&&textWidget->getFontBold()!=g_font_bold)
    {
        font_bold=property_create_subproperty_number(font, "font_bold", g_font_bold);
    }
    property_set_value_number(font_bold, textWidget->getFontBold());

    struct property *font_italic = property_get_subproperty(font, "font_italic");
    if(!font_italic&&textWidget->getFontItalic()!=g_font_italic)
    {
        font_italic=property_create_subproperty_number(font, "font_italic", g_font_italic);
    }
    property_set_value_number(font_italic, textWidget->getFontItalic());

    struct property *font_underLine = property_get_subproperty(font, "font_underline");
    if(!font_underLine&&textWidget->getFontUnderLine()!=g_font_underline)
    {
        font_underLine=property_create_subproperty_number(font, "font_underline", g_font_underline);
    }
    property_set_value_number(font_underLine, textWidget->getFontUnderLine());

    struct property *font_spacing = property_get_subproperty(font, "font_letterSpacing");
    if(!font_spacing&&textWidget->getFontKerning()!=g_font_letterSpacing)
    {
        font_spacing=property_create_subproperty_number(font, "font_letterSpacing", g_font_letterSpacing);
    }
    property_set_value_number(font_spacing, textWidget->getFontKerning());

    struct property *fontColor_pro = property_get_subproperty(font, "font_color");
    if(!fontColor_pro&&textWidget->getFontColor()!=g_font_color)
    {
        fontColor_pro=property_create_subproperty_color(font, "font_color", g_font_color);
    }
    property_set_value_color(fontColor_pro, textWidget->getFontColor());

    struct property *position_pro = property_get_subproperty(font, "text_position");
    if(!position_pro&&textWidget->getTextPosition()!=g_text_position)
    {
        position_pro=property_create_subproperty_number(font, "text_position", g_text_position);
    }
    property_set_value_number(position_pro, textWidget->getTextPosition());

    property_set_value_ustring(lanPro, (textWidget->getTextEditText()).toUtf8().constData());

    // 图片
    image_pro = property_get_subproperty(status_pro, "image");
    if(!image_pro&&imageWidget->m_imagePath!="")
    {
        image_pro=property_create_subproperty_ustring(status_pro, "image", "");
        property_create_subproperty_ustring(image_pro, "DRimage", "");
    }
    property_set_value_ustring(image_pro, imageWidget->m_imagePath.toUtf8().constData());
    //yangjindong--------------------

    QFile file(imageWidget->m_imagePath);
    if (!file.exists() && interface_adapter::getInstance()->m_imageMap.value(imageWidget->m_imagePath).isNull())
    {
        property_set_value_ustring(image_pro, "");
    }
}

void BitButtonProDialog::getGlobalFont()
{
    g_font_family = tools_getGlobalFont().family();
    g_font_pixelSize = tools_getGlobalFont().pixelSize();
    g_font_bold = tools_getGlobalFont().bold();
    g_font_italic = tools_getGlobalFont().italic();
    g_font_underline = tools_getGlobalFont().underline();
    g_font_letterSpacing = tools_getGlobalFont().letterSpacing();
    g_font_color = tools_getGlobalFontColor();
    g_text_position = tools_getGlobalFontTextPos();
}

void BitButtonProDialog::colorValueChanged(const QColor &value)
{
    struct property * childPro0 = NULL;
    struct property * childPro1 = NULL;
    struct property * status0_pro = m_preView0->getItemPropertyChanged("status0");
    struct property * status1_pro = m_preView1->getItemPropertyChanged("status1");
    //yangjindong--------------------
    QColor initbackgroundColor;
    if (m_name == "Bit Button")
        initbackgroundColor = QColor(238,238,238,255);
    else
        initbackgroundColor = QColor(192,192,192,255);
    if (m_borderBtn0 == sender())
    {
        if(!property_get_subproperty(status0_pro, "border color"))
        {
            property_create_subproperty_color(status0_pro, "border color", QColor(255,255,255,255));
        }
        childPro0 = m_preView0->getItemPropertyChanged(status0_pro, "border color");
    }
    else if (m_foregroundBtn0 == sender())
    {
        if(!property_get_subproperty(status0_pro, "foreground color"))
        {
            property_create_subproperty_color(status0_pro, "foreground color", QColor(0,0,0,255));
        }
        childPro0 = m_preView0->getItemPropertyChanged(status0_pro, "foreground color");
    }
    else if (m_backgroundBtn0 == sender())
    {
        if(!property_get_subproperty(status0_pro, "background color"))
        {
            property_create_subproperty_color(status0_pro, "background color", initbackgroundColor);
        }
        childPro0 = m_preView0->getItemPropertyChanged(status0_pro, "background color");
    }
    else if (m_borderBtn1 == sender())
    {
        if(!property_get_subproperty(status1_pro, "border color"))
        {
            property_create_subproperty_color(status1_pro, "border color", QColor(255,255,255,255));
        }
        childPro1 = m_preView1->getItemPropertyChanged(status1_pro, "border color");
    }
    else if (m_foregroundBtn1 == sender())
    {
        if(!property_get_subproperty(status1_pro, "foreground color"))
        {
            property_create_subproperty_color(status1_pro, "foreground color", QColor(0,0,0,255));
        }
        childPro1 = m_preView1->getItemPropertyChanged(status1_pro, "foreground color");
    }
    else if (m_backgroundBtn1 == sender())
    {
        if(!property_get_subproperty(status1_pro, "background color"))
        {
            property_create_subproperty_color(status1_pro, "background color", initbackgroundColor);
        }
        childPro1 = m_preView1->getItemPropertyChanged(status1_pro, "background color");
    }
    //yangjindong--------------------

    if (childPro0)
    {
        m_preView0->setItemPropertyChanged(childPro0, value);
    }

    if (childPro1)
    {
        m_preView1->setItemPropertyChanged(childPro1, value);
    }
}

void BitButtonProDialog::textValuesChanged(const QString &name,const QVariant &value)
{
    if (m_bTextProChangeIn)
        return;

    getGlobalFont();
    int langCount = 0;
    int langID = tools_getSystemCurrentLanguage();//m_prj->getSystemCurrentLanguage(langCount);
    QString langName = "Language" + QString::number(langID);
    //yangjindong--------------------
    if (m_textWidget0 == sender())
    {
        struct property * status0_pro = m_preView0->getItemPropertyChanged("status0");
        struct property * textPro0 = m_preView0->getItemPropertyChanged(status0_pro, "text");
        struct property * LangPro0 = m_preView0->getItemPropertyChanged(textPro0, langName.toUtf8().constData());
        if (LangPro0)
        {
            struct property *fontPro = m_preView0->getItemPropertyChanged(LangPro0, "font");
            struct property *changedPro = NULL;
            if( name == "font_family")
            {
                if(!property_get_subproperty(fontPro, "font_family"))
                {
                    property_create_subproperty_ustring(fontPro, "font_family", g_font_family.toUtf8().constData());
                }
                if(fontPro)
                    changedPro = m_preView0->getItemPropertyChanged(fontPro, "font_family");
                if(changedPro)
                    m_preView0->setItemPropertyChanged(changedPro, value.toString());
            }
            else if(name == "font_pixelSize")
            {
                if(!property_get_subproperty(fontPro, "font_pixelSize"))
                {
                    property_create_subproperty_number(fontPro, "font_pixelSize", g_font_pixelSize);
                }
                if(fontPro)
                    changedPro = m_preView0->getItemPropertyChanged(fontPro,"font_pixelSize");
                if(changedPro)
                    m_preView0->setItemPropertyChanged(changedPro, value.toInt());
            }
            else if(name == "font_letterSpacing")
            {
                if(!property_get_subproperty(fontPro, "font_letterSpacing"))
                {
                    property_create_subproperty_number(fontPro, "font_letterSpacing", g_font_letterSpacing);
                }
                if(fontPro)
                    changedPro = m_preView0->getItemPropertyChanged(fontPro,"font_letterSpacing");
                if(changedPro)
                    m_preView0->setItemPropertyChanged(changedPro, value.toInt());
            }
            else if(name == "font_bold")
            {
                if(!property_get_subproperty(fontPro, "font_bold"))
                {
                    property_create_subproperty_number(fontPro, "font_bold", g_font_bold);
                }
                if(fontPro)
                    changedPro = m_preView0->getItemPropertyChanged(fontPro, "font_bold");
                if(changedPro)
                    m_preView0->setItemPropertyChanged(changedPro,value.toInt());
            }
            else if(name == "font_italic")
            {
                if(!property_get_subproperty(fontPro, "font_italic"))
                {
                    property_create_subproperty_number(fontPro, "font_italic", g_font_italic);
                }
                if(fontPro)
                    changedPro = m_preView0->getItemPropertyChanged(fontPro, "font_italic");
                if(changedPro)
                    m_preView0->setItemPropertyChanged(changedPro,value.toInt());
            }
            else if(name == "font_underline")
            {
                if(!property_get_subproperty(fontPro, "font_underline"))
                {
                    property_create_subproperty_number(fontPro, "font_underline", g_font_underline);
                }
                if(fontPro)
                    changedPro = m_preView0->getItemPropertyChanged(fontPro, "font_underline");
                if(changedPro)
                    m_preView0->setItemPropertyChanged(changedPro,value.toInt());
            }
            else if(name == "text_position")
            {
                if(!property_get_subproperty(fontPro, "text_position"))
                {
                    property_create_subproperty_number(fontPro, "text_position", g_text_position);
                }
                if(fontPro)
                    changedPro = m_preView0->getItemPropertyChanged(fontPro, "text_position");
                if(changedPro)
                    m_preView0->setItemPropertyChanged(changedPro,value.toInt());
            }
            else if(name == "font_color")
            {
                if(!property_get_subproperty(fontPro, "font_color"))
                {
                    property_create_subproperty_color(fontPro, "font_color", g_font_color);
                }
                if(fontPro)
                    changedPro = m_preView0->getItemPropertyChanged(fontPro, "font_color");
                if(changedPro)
                    m_preView0->setItemPropertyChanged(changedPro,value.value<QColor>());
            }
            else if(name == "text")
            {
                m_preView0->setItemPropertyChanged(LangPro0, value.toString());
            }
        }
    }
    else if (m_textWidget1 == sender())
    {
        struct property * status1_pro = m_preView1->getItemPropertyChanged("status1");
        struct property * textPro1 = m_preView1->getItemPropertyChanged(status1_pro, "text");
        struct property * LangPro1 = m_preView1->getItemPropertyChanged(textPro1, langName.toUtf8().constData());
        if (LangPro1)
        {
            struct property *fontPro = m_preView1->getItemPropertyChanged(LangPro1, "font");
            struct property *changedPro = NULL;
            if( name == "font_family")
            {
                if(!property_get_subproperty(fontPro, "font_family"))
                {
                    property_create_subproperty_ustring(fontPro, "font_family", g_font_family.toUtf8().constData());
                }
                if(fontPro)
                    changedPro = m_preView1->getItemPropertyChanged(fontPro, "font_family");
                if(changedPro)
                    m_preView1->setItemPropertyChanged(changedPro, value.toString());
            }
            else if(name == "font_pixelSize")
            {
                if(!property_get_subproperty(fontPro, "font_pixelSize"))
                {
                    property_create_subproperty_number(fontPro, "font_pixelSize", g_font_pixelSize);
                }
                if(fontPro)
                    changedPro = m_preView1->getItemPropertyChanged(fontPro,"font_pixelSize");
                if(changedPro)
                    m_preView1->setItemPropertyChanged(changedPro, value.toInt());
            }
            else if(name == "font_letterSpacing")
            {
                if(!property_get_subproperty(fontPro, "font_letterSpacing"))
                {
                    property_create_subproperty_number(fontPro, "font_letterSpacing", g_font_letterSpacing);
                }
                if(fontPro)
                    changedPro = m_preView1->getItemPropertyChanged(fontPro,"font_letterSpacing");
                if(changedPro)
                    m_preView1->setItemPropertyChanged(changedPro, value.toInt());
            }
            else if(name == "font_bold")
            {
                if(!property_get_subproperty(fontPro, "font_bold"))
                {
                    property_create_subproperty_number(fontPro, "font_bold", g_font_bold);
                }
                if(fontPro)
                    changedPro = m_preView1->getItemPropertyChanged(fontPro, "font_bold");
                if(changedPro)
                    m_preView1->setItemPropertyChanged(changedPro,value.toInt());
            }
            else if(name == "font_italic")
            {
                if(!property_get_subproperty(fontPro, "font_italic"))
                {
                    property_create_subproperty_number(fontPro, "font_italic", g_font_italic);
                }
                if(fontPro)
                    changedPro = m_preView1->getItemPropertyChanged(fontPro, "font_italic");
                if(changedPro)
                    m_preView1->setItemPropertyChanged(changedPro,value.toInt());
            }
            else if(name == "font_underline")
            {
                if(!property_get_subproperty(fontPro, "font_underline"))
                {
                    property_create_subproperty_number(fontPro, "font_underline", g_font_underline);
                }
                if(fontPro)
                    changedPro = m_preView1->getItemPropertyChanged(fontPro, "font_underline");
                if(changedPro)
                    m_preView1->setItemPropertyChanged(changedPro,value.toInt());
            }
            else if(name == "text_position")
            {
                if(!property_get_subproperty(fontPro, "text_position"))
                {
                    property_create_subproperty_number(fontPro, "text_position", g_text_position);
                }
                if(fontPro)
                    changedPro = m_preView1->getItemPropertyChanged(fontPro, "text_position");
                if(changedPro)
                    m_preView1->setItemPropertyChanged(changedPro,value.toInt());
            }
            else if(name == "font_color")
            {
                if(!property_get_subproperty(fontPro, "font_color"))
                {
                    property_create_subproperty_color(fontPro, "font_color", g_font_color);
                }
                if(fontPro)
                    changedPro = m_preView1->getItemPropertyChanged(fontPro, "font_color");
                if(changedPro)
                    m_preView1->setItemPropertyChanged(changedPro,value.value<QColor>());
            }
            else if(name == "text")
            {
                m_preView1->setItemPropertyChanged(LangPro1, value.toString());
            }
        }
    }
    //yangjindong--------------------
}

void BitButtonProDialog::imageValuesChanged(const QString & /*name*/, const QVariant &value)
{
    //yangjindong--------------------
    if (m_imageWidget0 == sender())
    {
        struct property * status0_pro = m_preView0->getItemPropertyChanged("status0");
        struct property *changedPro0 = NULL;
        if (status0_pro)
        {
            changedPro0 = m_preView0->getItemPropertyChanged(status0_pro, "image");
            if(!changedPro0)
            {
                changedPro0=property_create_subproperty_color(status0_pro, "image", "");
            }
            if (changedPro0)
            {
                m_preView0->setItemPropertyChanged(changedPro0, value.toString());
            }
        }
    }
    else if (m_imageWidget1 == sender())
    {
        struct property * status1_pro = m_preView1->getItemPropertyChanged("status1");
        struct property *changedPro1 = NULL;
        if (status1_pro)
        {
            changedPro1 = m_preView1->getItemPropertyChanged(status1_pro, "image");
            if(!changedPro1)
            {
                changedPro1=property_create_subproperty_color(status1_pro, "image", "");
            }
            if (changedPro1)
            {
                m_preView1->setItemPropertyChanged(changedPro1, value.toString());
            }
        }
    }
    //yangjindong--------------------
}

void BitButtonProDialog::brushStyleValueChanged(const int & value)
{
    //yangjindong--------------------
    // 修改填充类型
    if (m_brushStyleCombox0 == sender())
    {
        struct property * status0_pro = m_preView0->getItemPropertyChanged("status0");
        struct property *changedPro0 = NULL;
        if (status0_pro)
        {
            if(!property_get_subproperty(status0_pro, "pattern"))
            {
                pattern_pro = property_create_subproperty_number(status0_pro, "pattern", 1);
            }
            changedPro0 = m_preView0->getItemPropertyChanged(status0_pro, "pattern");
            if (changedPro0)
            {
                m_preView0->setItemPropertyChanged(changedPro0, value);
            }
        }
    }
    else if (m_brushStyleCombox1 == sender())
    {
        struct property * status1_pro = m_preView1->getItemPropertyChanged("status1");
        struct property *changedPro1 = NULL;
        if (status1_pro)
        {
            if(!property_get_subproperty(status1_pro, "pattern"))
            {
                pattern_pro = property_create_subproperty_number(status1_pro, "pattern", 1);
            }
            changedPro1 = m_preView1->getItemPropertyChanged(status1_pro, "pattern");
            if (changedPro1)
            {
                m_preView1->setItemPropertyChanged(changedPro1, value);
            }
        }
    }
    //yangjindong--------------------
}

/*功能增加，新增脚本功能，20180116，begin*/
void BitButtonProDialog::macroEdit()
{
    QVariantList varlist;
    if(0 == m_macroBox->currentIndex())
    {
        varlist.append(QVariant::fromValue(m_pressMacro));
        varlist.append(QVariant::fromValue(m_macroPressFontStr));
        varlist.append(QVariant::fromValue(m_macroPressFontSize));
        QVariant var(varlist);
        ((UAppBase*)qApp)->_execFuncFromID(hmi_extend_script_editDialog,var);
        varlist = var.toList();
        if(3 == varlist.size())
        {
            m_pressMacro = varlist.at(0).toString();
            m_macroPressFontStr = varlist.at(1).toString();
            m_macroPressFontSize = varlist.at(2).toInt();
        }
    }
    else
    {
        varlist.append(QVariant::fromValue(m_releaseMacro));
        varlist.append(QVariant::fromValue(m_macroReleaseFontStr));
        varlist.append(QVariant::fromValue(m_macroReleaseFontSize));
        QVariant var(varlist);
        ((UAppBase*)qApp)->_execFuncFromID(hmi_extend_script_editDialog,var);
        varlist = var.toList();
        if(3 == varlist.size())
        {
            m_releaseMacro = varlist.at(0).toString();
            m_macroReleaseFontStr = varlist.at(1).toString();
            m_macroReleaseFontSize = varlist.at(2).toInt();
        }
    }
}

void BitButtonProDialog::macroClicked()
{
    if(m_macroFlag->isChecked())
    {
          m_macroBox->setVisible(true);
          m_macroEdit->setVisible(true);
    }
    else
    {
        m_macroBox->setVisible(false);
        m_macroEdit->setVisible(false);
    }
}
/*功能增加，新增脚本功能，20180116，end*/
/*【新增功能】_文本控件和位指示灯添加闪烁功能_chiji_20190208_Begin*/
void BitButtonProDialog::flashCheSlot(int index)
{
    //yangjindong--------------------
    struct property * thePro = object_get_property(m_object,"flashLamp");
    if(!thePro&&index!=0)
        thePro = object_create_property_number(m_object, "flashLamp", 0);
    property_set_value_uint32(thePro,index);
    //yangjindong--------------------
    if(index == 0)
    {
        m_autoChangeSpinBox->setEnabled(false);
    }
    else
    {
        m_autoChangeSpinBox->setEnabled(true);
    }
}
/*【新增功能】_文本控件和位指示灯添加闪烁功能_chiji_20190208_End*/
