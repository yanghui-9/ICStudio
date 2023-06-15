#ifndef SCENEPROPERTYEDITDLG_H
#define SCENEPROPERTYEDITDLG_H
#include <QDialog>
#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>
#include <QRadioButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QJsonObject>

class UWidgetScene;
class UScenePropertyEditDlg : public QDialog
{
    Q_OBJECT
public:
    UScenePropertyEditDlg(UWidgetScene * scene,QWidget *parent = nullptr);

    //初始化UI显示
    void InitUI();

    //刷新数据
    void RefreshUI(QJsonObject &sceneObj);

protected slots:
    //脚本勾选处理槽
    void scriptCheckboxSlot(int state);
    //保存
    void saveSlot();
    //颜色选择
    void colorSelectSlot();
    //图片选择
    void imgSelectSlot();
    //背景类型切换
    void bgTypeChangeSlot();

private:
    QGroupBox * m_scrit;
    QCheckBox * pressScript;
    QComboBox * pressCombox;
    QCheckBox * releaseScript;
    QComboBox * releaseCombox;
    QCheckBox * cycleScript;
    QComboBox * cycleCombox;
    QLineEdit * cycTimeEdit;

    QGroupBox * m_window;
    QLineEdit * lineX;
    QLineEdit * lineY;
    QLineEdit * lineW;
    QLineEdit * lineH;

    QRadioButton * colorRadio;
    QPushButton * colorBtn;
    QRadioButton * picRadio;
    QPushButton * picBtn;

    QColor m_bgColor;
    QStringList m_scriptList;

    UWidgetScene * m_scene = nullptr;

public:
    QJsonObject m_sceneObj;
};

#endif // SCENEPROPERTYEDITDLG_H
