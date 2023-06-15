#ifndef ERRCODEHELP_H
#define ERRCODEHELP_H

#include <map>
#include "tools.h"



struct ErrCodeHelpInfo
{
    ErrCodeHelpInfo(){};
    ErrCodeHelpInfo(const std::string& help) :
    sHelp(help)
    {};

    std::string sErrCode = "";  //错误码
    std::string sHelp = "";     //帮助信息
};

class _TOOLS ErrCodeHelp
{
public:
    virtual ~ErrCodeHelp();

    /*
     * 添加错误码的帮助信息
     * @params sErrCode : 错误码
     * @params helpInfo : 错误码对应的帮助信息
     * @return bool : true 添加成功     false 添加失败
     */
    static bool addErrCodeHelp(const std::string& sErrCode, const ErrCodeHelpInfo& helpInfo);

    /*
     * 获取错误码的帮助信息
     * @params sErrCode : 错误码
     * @return ErrCodeHelpInfo : 错误码对应的帮助信息
     */
    static ErrCodeHelpInfo getErrCodeHelp(const std::string& sErrCode);

    /*
     * 获取错误码的帮助信息
     * @params sErrCode : 错误码
     * @params bOk : true 获取成功      false 获取失败
     * @return ErrCodeHelpInfo : 错误码对应的帮助信息
     */
    static ErrCodeHelpInfo getErrCodeHelp(const std::string& sErrCode, bool& bOk);

private:
    explicit ErrCodeHelp();

private:
    //@std::map<错误码, 错误码帮助信息>
    static std::map<std::string, ErrCodeHelpInfo> ms_mapErrCode;
};






#endif //!ERRCODEHELP_H
