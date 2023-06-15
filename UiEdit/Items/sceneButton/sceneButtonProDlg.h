#ifndef SCENEBUTTONPRODLG_H
#define SCENEBUTTONPRODLG_H
#include <QComboBox>
#include "../ItemProEditDlgBase.h"

class item_base;
class BackgroundSettingsW;
class LabelBase;
class sceneButtonProDlg : public ItemProEditDlgBase
{
    Q_OBJECT
public:
    sceneButtonProDlg(item_base *item, QWidget * parent = nullptr);
    virtual ~sceneButtonProDlg(){}

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
};

#endif
