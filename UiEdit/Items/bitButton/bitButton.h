#ifndef UBitButton_H
#define UBitButton_H
#include <QPushButton>
#include "../item_base.h"

class UBitButton : public item_base
{
    Q_OBJECT
public:
    UBitButton(QWidget *parent = nullptr);
    virtual~UBitButton(){}
    void initWidget();
    virtual void itemPaintEvent(QPaintEvent *event);

    //结果调用
    virtual bool update_result(int flag = 0, void *reslut = nullptr);

    //地址信息
    virtual bool update_addr(QList<Protocol::AddrInfoForRW>& addrList);

    //on off 变化值回调
    void addrChangeCB(const Protocol::AddrInfoForRW &, int32_t code);
private:
    //QImage *m_on = nullptr;
    //QImage *m_off = nullptr;
    std::shared_ptr<QImage> m_on;
    std::shared_ptr<QImage> m_off;
    bool m_curStatus = false;
};




#endif // UBitButton_H
