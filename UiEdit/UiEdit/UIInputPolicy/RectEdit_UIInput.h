#ifndef RectEdit_UIInput_H
#define RectEdit_UIInput_H

#include "../UIInputPolicy/IUIInputPolicy.h"
#include "../UWidgetScene.h"

#define Mouse_Type_Rect 10
class RectEdit_UIInput : public IUIInputPolicy
{
public:
    enum mouseType
    {
        Horizontal_Left = 0,
        Horizontal_Right,
        Vertical_Up,
        Vertical_Down,
        Upper_Left,
        Lower_Right,
        Upper_Right,
        Lower_Left,
        mouse_nor = 0xff
    };

    RectEdit_UIInput();
    virtual ~RectEdit_UIInput(){}

    virtual bool mousePress( QMouseEvent * evt );
    virtual bool mouseMove( QMouseEvent  * evt );
    virtual bool mouseRelease( QMouseEvent * evt );
    virtual void setScreen(UWidgetScene * scr);
    virtual bool keyPressEvent( QKeyEvent * event ) ;

    //处理组合控件大小变化
    static void DealCombineItem(QWidget *item);


protected:
    //保证控件最小大小
    void ItemMiniSize(QWidget * item);
    //处理鼠标形状
    void DealMouseType(const QPoint &pos);
    void dealMouseType(const QPointF &curPos, QRectF normalrectF);

public:
    UWidgetScene * mScreen;
    mouseType m_mouseIndex;
    bool m_pressFlag;
    QPoint m_movePos;
    QPoint m_PressPos; //TODO:保存鼠标点击时的坐标，用于拖拽复制功能
    QRect m_oldRect;   //控件尺寸变化之前
    QWidget * m_selectedItem = nullptr;

    void doHorStretch(int index, QPoint pos, QWidget * item  );   // 水平伸缩操作
    void doVerStretch(int index, QPoint pos, QWidget * item  );   // 垂直伸缩操作
    void doFDiagStretch(int index, QPoint pos, QWidget * item  ); // 对角线(左上-右下)伸缩操作
    void doBDiagStretch(int index, QPoint pos, QWidget * item  ); // 对角线(左下-右上)伸缩操作
    void doItemMove( QPointF dis, QWidget * item );                // 图形移动操作
    double getValid_X(double value);
    double getValid_Y(double value);
};

#endif // __RECT__EDIT__UI__INPUT__H__HEADER__INCLUDE__
