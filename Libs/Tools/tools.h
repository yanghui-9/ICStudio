#ifndef tools_h
#define tools_h
#include <string>
#include <vector>

#if defined(TOOLS_LIB)
#  define _TOOLS  __declspec(dllexport)
#else
#  define _TOOLS  __declspec(dllimport)
#endif


//导出c函数接口

//工程配置相关
//导出导入工程文件（带校验）
extern "C" _TOOLS bool Tools_ExportProject(const std::string &sFileName);
extern "C" _TOOLS bool Tools_ImportProject(const std::string &sFileName);
//检查工程文件配置是否损坏
extern "C" _TOOLS bool Tools_CheckProject();
//文件夹复制
extern "C" _TOOLS bool Tools_CopyDirectory(const std::string &fromDir, const std::string &toDir, bool bCoverIfFileExists);

//通用工具接口
//汉字编码转换
extern "C" _TOOLS void Tools_GBK2UTF8(std::string *inStr);
//检查目录大小
extern "C" _TOOLS uint64_t Tools_GetDirSize(std::string *sdirPath);



#endif


