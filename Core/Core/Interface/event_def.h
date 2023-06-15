#ifndef _EVENT_DEF_H_
#define _EVENT_DEF_H_

#include "IApp.h"
#include <QVariant>
/*********************************************************
 *
 * 自定义消息，消息号字段统一定义
 *
 */

/***************************事件类型
 * 各个模块定义事件类型的地方，全局统一管理
 */
enum Event_Type
{
    //工程管理模块(0x0-0xFE)
    Event_Project_Load = 0x0,//工程加载
    Event_Project_Unload,//工程关闭
    Event_Project_Compile,//工程编译
    Event_Project_LanguageChange,//多语言切换
};


class ZObject;
class HEvent
{
public:
    explicit HEvent(Event_Type type,const QVariant &var,
                    ZObject * send = nullptr,
                    Event_Pri_Type pri = Event_Nor,
                    bool isThread = false)
        :_var(var),_pri(pri),_send(send),_type(type),_isThread(isThread){}

    QVariant _var;        //事件附带的数据
    unsigned char _pri;   //事件优先级
    ZObject *_send; //事件发送者
    Event_Type _type;           //事件类型
    bool _isThread = false;    //事件是否在线程中分发
};

#endif // _HMI_EVENT_DEF_H_
