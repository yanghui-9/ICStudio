#ifndef BITBUTTONPRODLG_H
#define BITBUTTONPRODLG_H
#include "../ItemProEditDlgBase.h"

class item_base;
class bitButtonProDlg : public ItemProEditDlgBase
{
public:
    bitButtonProDlg(item_base *item, QWidget * parent = nullptr);
    virtual ~bitButtonProDlg(){}

protected:
    //自定义属性页面
    virtual void InitCustomPage();
    virtual void RefreshCustomPage();
    virtual void SaveCustomPage();

private:
    QPushButton * varBtn;
    QPushButton * offPicBtn;
    QPushButton * onPicBtn;
};

#endif // BITBUTTONPRODLG_H
