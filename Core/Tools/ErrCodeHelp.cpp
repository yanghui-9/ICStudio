#include "ErrCodeHelp.h"



std::map<std::string, ErrCodeHelpInfo> ErrCodeHelp::ms_mapErrCode;

ErrCodeHelp::ErrCodeHelp()
{

}

ErrCodeHelp::~ErrCodeHelp()
{

}

bool ErrCodeHelp::addErrCodeHelp(const std::string &sErrCode, const ErrCodeHelpInfo &helpInfo)
{
    if(ms_mapErrCode.find(sErrCode) != ms_mapErrCode.end())
        return false;

    auto tempHelpInfo = helpInfo;
    tempHelpInfo.sErrCode = sErrCode;

    ms_mapErrCode.insert(std::make_pair(sErrCode, tempHelpInfo));
    return true;
}

ErrCodeHelpInfo ErrCodeHelp::getErrCodeHelp(const std::string& sErrCode)
{
    bool bOk = false;
    return getErrCodeHelp(sErrCode, bOk);
}

ErrCodeHelpInfo ErrCodeHelp::getErrCodeHelp(const std::string& sErrCode, bool &bOk)
{
    if(ms_mapErrCode.find(sErrCode) == ms_mapErrCode.end())
    {
        bOk = false;
        return ErrCodeHelpInfo();
    }

    bOk = true;
    return ms_mapErrCode[sErrCode];
}
