#ifndef BITBUTTONPRODIALOG_H
#define BITBUTTONPRODIALOG_H
#include <QDialog>
#include "../../../basics_model/device_struct/include/cobject.h"
#include "../../../basics_model/device_system/BrushStyleComboBox.h"
#include "../../../basics_model/device_system/PropertyDialogGlobalWidget.h"
#include "../../../basics_model/device_system/ColorSelectPushButton.h"
#include "../PreviewWidget.h"
#include "../SeniorSettingWidget.h"
#include "../VisibilitySettingWidget.h"

class BitButtonProDialog : public QDialog
{
    Q_OBJECT
public:
    BitButtonProDialog(const QString & name, QWidget *parent = 0);
    ~BitButtonProDialog();

public:
    void initWidget();
    void refreshUIfromData(struct object *obj);

    bool m_bTextProChangeIn;

protected:
    void initGenaralTab(QWidget *w);
    void initLabelTab(QWidget *w);

    void getPropertyValue(struct property *pro,PROVALUE *value);
    void updateStatusValue(const PROVALUE *value);
    void savePropertyValue(PROVALUE *proValue,struct property *status_pro, TextWidget *textWidget, ImageWidget *imageWidget);

    void getGlobalFont();

protected slots:
    void currentIndexSlot(int index);

    void confirmSlot();
    void helpSlot();

    void colorValueChanged(const QColor &);
    void textValuesChanged(const QString &,const QVariant &);
    void imageValuesChanged(const QString &,const QVariant &);
    void brushStyleValueChanged(const int &);
    /*功能增加，新增脚本功能，20180116，begin*/
    void macroEdit();
    void macroClicked();
    /*功能增加，新增脚本功能，20180116，end*/
    /*【新增功能】_文本控件和位指示灯添加闪烁功能_chiji_20190208_Begin*/
    void flashCheSlot(int);
signals:
    void previewValueChanged(const QString &,const QVariant &);

private:
    QString m_name;
    struct object * m_object;
    QCheckBox *m_twinkleChebox;
    QSpinBox * m_autoChangeSpinBox;
    QLabel * m_autoChangeLabel;
    QLabel * m_timeLabel;
    /*【新增功能】_文本控件和位指示灯添加闪烁功能_chiji_20190208_End*/
    /*功能增加，新增脚本功能，20180116，begin*/
    QRadioButton * m_macroFlag;
    QComboBox * m_macroBox;
    QPushButton * m_macroEdit;
    QString m_pressMacro;
    QString m_releaseMacro;
    QMap<QString,QString>m_links;
    QString m_macroPressFontStr;
    int m_macroPressFontSize;
    QString m_macroReleaseFontStr;
    int m_macroReleaseFontSize;
    /*功能增加，新增脚本功能，20180116，end*/

    ColorSelectPushButton * m_borderBtn0;
    ColorSelectPushButton * m_foregroundBtn0;
    ColorSelectPushButton * m_backgroundBtn0;
    ColorSelectPushButton * m_borderBtn1;
    ColorSelectPushButton * m_foregroundBtn1;
    ColorSelectPushButton * m_backgroundBtn1;

    BrushStyleComboBox * m_brushStyleCombox0;
    BrushStyleComboBox * m_brushStyleCombox1;

    QPushButton *m_confirmBtn;
    QPushButton *m_cancelBtn;
    QPushButton *m_helpBtn;

    QButtonGroup *m_functionsGroup;

    QWidget * bitNumberWidget;
    QComboBox * m_bitNumberComboBox;

    PreviewWidget *m_preView0;
    PreviewWidget *m_preView1;
    AddressWidget *m_addressWidget;

    TextWidget *m_textWidget0;
    ImageWidget *m_imageWidget0;
    TextWidget *m_textWidget1;
    ImageWidget *m_imageWidget1;

    PROVALUE *proValue0;
    PROVALUE *proValue1;

    struct property * currentStatus_pro;
    struct property * bitNumber_pro;
    struct property * functions_pro;
    struct property * borderColor_pro;
    struct property * backgroundColor_pro;
    struct property * foregroundColor_pro;
    struct property * pattern_pro;
    struct property * dataType_pro;
    struct property * text_pro;
    struct property * lan_pro;
    struct property * image_pro;
    struct property * monitor_pro;
    struct property * maddress_pro;
    struct property * waddress_pro;
    struct property * sameaddress_pro;
    struct object * writeAddress_obj;
    struct object * monitorAddress_obj;

    QFontDatabase fontDatabase;

    //Project *m_prj;
    QString g_font_family;
    int g_font_pixelSize;
    bool g_font_bold;
    bool g_font_italic;
    bool g_font_underline;
    int g_font_letterSpacing;
    QColor g_font_color;
    int g_text_position;

public:
    SeniorSettingWidget *m_seniorWidget;
    VisibilitySettingWidget * m_visibilityWidget;
};

#endif // BITBUTTONPRODIALOG_H
