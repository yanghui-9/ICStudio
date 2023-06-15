#ifndef INTERFACE_ALL_HEAD_H
#define INTERFACE_ALL_HEAD_H


/*********************************************************
 *
 * 自定义功能，功能ID字段统一定义
 * 各个模块定义功能ID的地方，全局统一管理
 */

enum Extend_Func_Id_Type
{
    //画面控件编辑视图区域(0x0-0xFF)
    Extend_View_SwitchScene = 0x0,//切换画面

    //通讯模块(0x100-0x1FF)

    //脚本模块(0x200-0x2FF)
    Extend_Script_Register = 0x200,//void RegisterScriptFunc(ScriptFunc func,std::string name);
    Extend_Script_RunFromName = 0x201,//void RunScriptFromName(const std::string &name,bool isSync)
};

#endif // INTERFACE_ALL_HEAD_H
