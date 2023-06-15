#ifndef USceneButton_H
#define USceneButton_H
#include <QPushButton>
#include "../item_base.h"

class USceneButton : public item_base
{
    Q_OBJECT
public:
    USceneButton(QWidget *parent = nullptr);
    virtual~USceneButton(){}
    void initWidget();
    virtual void itemPaintEvent(QPaintEvent *event);

    //结果调用
    virtual bool update_result(int flag = 0);

    //地址信息
    virtual bool update_addr(QList<Protocol::AddrInfoForRW>& addrList);

    //变化值回调
    void addrChangeCB(const Protocol::AddrInfoForRW &, int32_t code);

protected:
    //按下切换
    void mousePressEvent(QMouseEvent *event);

private:
    //std::shared_ptr<QImage> m_on;
    //std::shared_ptr<QImage> m_off;
};




#endif // USceneButton_H
