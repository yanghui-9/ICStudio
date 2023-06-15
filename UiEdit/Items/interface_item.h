#ifndef INTERFACE_ITEM
#define INTERFACE_ITEM
#include <QString>
#include <QGraphicsItem>
#include <QList>
#include <QAction>
#include "protocol_type.h"

#define ITEM_GET_FLAG(flag,num) ((flag>>num)&0x1)
#define ITEM_SET_FLAG(flag,num) (flag|(0x1 << num))

#define Item_Update_Flag_Loop             (0)      //循环调用
#define Item_Update_Flag_Keyboard         (1)      //键盘调用
#define Item_Update_Flag_AddrChangge      (2)      //绑定地址变化触发
#define Item_Update_Flag_SceneClose       (3)      //画面打开
#define Item_Update_Flag_SceneOpen        (4)      //画面关闭
#define Item_Update_Flag_ModeChange       (5)      //模式切换-运行编辑
#define Item_Update_Flag_ModeChange_V     (6)      //模式切换结果
#define Item_Update_Flag_Refresh          (7)      //主动刷新
#define Item_Update_Flag_ProEdit          (8)      //控件属性编辑

class interface_comm;
class interface_gallery;
class interface_variable_table;
class interface_item : public QWidget
{
public:
    interface_item(QWidget *parent):QWidget(parent){}
    virtual ~interface_item(){}

    //控件初始化接口
    virtual void initItem(std::shared_ptr<interface_comm> &comm,
                          std::shared_ptr<interface_gallery> &gallery,
                          std::shared_ptr<interface_variable_table> &variables){
        Q_UNUSED(comm);
        Q_UNUSED(gallery);
        Q_UNUSED(variables);
    }

    //计算控件绘制结果
    //flag默认0 flag
    //第0位为1表示固定刷新调用，直接切换状态
    //第1位为1表示键盘输入调用
    //第2位为1表示地址变化调用
    //第3位为1表示画面切换，关闭当前画面
    //第4位为1表示画面切换，显示当前画面
    //第5位为1表示运行编辑模式切换
    //第6位为1表示切换为运行模式，1表示编辑模式
    virtual bool update_result(int flag = 0){
        Q_UNUSED(flag);
        return false;
    }

    //获取控件地址和用户等级控制信息
    virtual bool update_addr(QList<Protocol::AddrInfoForRW>& addrList){
        Q_UNUSED(addrList);
        return false;
    }

    //无需返回-1,当该控件当前画面显示时按照循环时间循环定时调用该接口
    virtual int64_t getCyclicCallInterval(){
        return -1;
    }

    //编译检查接口
    virtual QString compile(int flag = 0){
        Q_UNUSED(flag);
        return "";
    }
};
#endif // INTERFACE_ITEM

