#ifndef RectCreate_UIInput_H
#define RectCreate_UIInput_H

#include "../UIInputPolicy/IUIInputPolicy.h"
#include "../UWidgetScene.h"

class RectCreate_UIInput : public IUIInputPolicy
{
public:
    RectCreate_UIInput();
    virtual ~RectCreate_UIInput(){}

    virtual void setScreen(UWidgetScene * scr);

    virtual bool mousePress(QMouseEvent *evt );
    virtual bool mouseMove( QMouseEvent  * evt );
    virtual bool mouseRelease( QMouseEvent * evt );
    virtual bool keyPressEvent( QKeyEvent * event ) ;

public:
    UWidgetScene * mScreen = nullptr;
    QWidget * item = nullptr;
    QMap< QString, int > preNameMap;
};

#endif // __RECTANGLE__CREATE__UI__INPUT__H__
