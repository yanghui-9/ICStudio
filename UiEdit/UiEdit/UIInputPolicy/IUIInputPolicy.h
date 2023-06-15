#ifndef IUIINPUTPOLICY_H
#define IUIINPUTPOLICY_H

#include <QtGui/QKeyEvent>
#include <QtCore/QPoint>
#include <QApplication>
class UWidgetScene;
class IUIInputPolicy
{
public:
    /* in */
    virtual void setScreen(UWidgetScene *) = 0;

    /* ui handler */
    virtual bool mousePress( QMouseEvent * evt ) = 0;
    virtual bool mouseMove( QMouseEvent * evt ) = 0;
    virtual bool mouseRelease( QMouseEvent* evt ) = 0;
    virtual bool keyPressEvent( QKeyEvent * event ) = 0;
};

#endif // IUIINPUTPOLICY_H
