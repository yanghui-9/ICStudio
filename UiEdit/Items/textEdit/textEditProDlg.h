#ifndef textEditProDlg_H
#define textEditProDlg_H
#include <QComboBox>
#include "../ItemProEditDlgBase.h"

class item_base;
class BackgroundSettingsW;
class LabelBase;
class textEditProDlg : public ItemProEditDlgBase
{
    Q_OBJECT
public:
    textEditProDlg(item_base *item, QWidget * parent = nullptr);
    virtual ~textEditProDlg(){}

protected:
    //自定义属性页面
    virtual void InitCustomPage();
    virtual void RefreshCustomPage();
    virtual void SaveCustomPage();

private:
    BackgroundSettingsW * m_bgW;
    LabelBase * m_label;
    QComboBox * m_sceneBox;
    QStringList m_sceneList;

    QPushButton *varBtn;
};

#endif
