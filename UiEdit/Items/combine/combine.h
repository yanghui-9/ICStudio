#ifndef combine_H
#define combine_H
#include <QPushButton>
#include "../item_base.h"

class combine : public item_base
{
    Q_OBJECT
public:
    combine(QWidget *parent = nullptr);
    virtual~combine(){}
    void initWidget();
    virtual void itemPaintEvent(QPaintEvent *event);

    //计算控件绘制结果
    //flag默认0 flag
    //第0位为1表示固定刷新调用，直接切换状态
    //第1位为1表示键盘输入调用
    //第2位为1表示地址变化调用
    //第3位为1表示画面切换，关闭当前画面
    //第4位为1表示画面切换，显示当前画面
    //第5位为1表示运行编辑模式切换
    //第6位为1表示切换为运行模式，1表示编辑模式
    virtual bool update_result(int flag = 0, void *reslut = nullptr);

    //地址信息
    virtual bool update_addr(QList<Protocol::AddrInfoForRW>& addrList);

    //on off 变化值回调
    void addrChangeCB(const Protocol::AddrInfoForRW &, int32_t code);

private:
    bool m_curStatus = false;
};




#endif // combine_H
