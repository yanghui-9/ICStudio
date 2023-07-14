#ifndef textEdit_H
#define textEdit_H
#include <QPushButton>
#include "../item_base.h"

class textEdit : public item_base
{
    Q_OBJECT
public:
    textEdit(QWidget *parent = nullptr);
    virtual~textEdit(){}
    void initWidget();

    //结果调用
    virtual bool update_result(int flag = 0, void *reslut = nullptr);

    //地址信息
    virtual bool update_addr(QList<Protocol::AddrInfoForRW>& addrList);

    //变化值回调
    void addrChangeCB(const Protocol::AddrInfoForRW &, int32_t code);

    //刷新
    void itemUpdate();

protected:
    //按下切换
    void mousePressEvent(QMouseEvent *event);

private:
    //std::shared_ptr<QImage> m_on;
    //std::shared_ptr<QImage> m_off;
    QString m_text = "0";
};




#endif // textEdit_H
