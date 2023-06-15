#ifndef Select_UIInput_H
#define Select_UIInput_H

#include "../UIInputPolicy/IUIInputPolicy.h"
#include "UWidgetScene.h"

class Select_UIInput : public IUIInputPolicy
{
public:
    Select_UIInput();
    virtual ~Select_UIInput(){}

    virtual bool mousePress( QMouseEvent * evt );
    virtual bool mouseMove( QMouseEvent * evt );
    virtual bool mouseRelease( QMouseEvent * evt );
    virtual void setScreen(UWidgetScene * scr);
    virtual bool keyPressEvent( QKeyEvent * event ) ;

protected:
    //初始化选择画布
    void InitSelectItem();

public:
    UWidgetScene * mScreen = nullptr;
    QPoint m_pressPos;
    bool m_pressFlag;
    bool m_mulselectFlag;
    QRect m_selectRect;
    bool m_pflag;
    QWidget *m_selectRectItem = nullptr;//选择画布
};

#endif // __SELECT__UI__INPUT__H__
