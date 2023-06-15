#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <vector>
#include <list>
#include <functional>
#include "protocol_global.h"
#include "protocol_type.h"

class ICommunication;
extern "C"
{

   /**
   *  @brief  获取协议参数
   *  @param[in]  config：协议默认参数
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail
   */
   PROTOCOLSHARED_EXPORT int32_t GetDeviceConfig(Protocol::Device_Config *config);

   /**
   *  @brief  初始化协议，支持动态调整参数
   *  @param[in]  comm：通讯接口智能指针 config：协议参数
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail
   */
   PROTOCOLSHARED_EXPORT int32_t InitDevice(std::shared_ptr<ICommunication> *comm, const Protocol::Device_Config *config);

   /**
   *  @brief  通讯一次，包含循环读取+异步写入+条件读取，默认写入优先级>条件读取>循环读取，优先级阈值默认为3（例如连续写入三次之后必须条件读一次）
   *  @param
   *  @return:
   *  -- 没有需要操作的事务，返回 Protocol_Process_Null
   *  -- 读取成功，返回  Protocol_Process_R_Suc
   *  -- 读取失败，返回  Protocol_Process_R_NoData
   *  -- 读取失败，返回  Protocol_Process_R_CheckError
   *  -- 写入成功，返回  Protocol_Process_W_Suc
   *  -- 写入失败，返回  Protocol_Process_W_NoData
   *  -- 写入失败，返回  Protocol_Process_W_CheckError
   */
   PROTOCOLSHARED_EXPORT int32_t Process();

   /**
   *  @brief 设置循环读取地址信息，地址整合优化分块并返回优化分块结果
   *  @param[in] addrVec：输入地址信息，输入优化分块结果  addType：操作类型，Protocol_Cover-覆盖模式 Protocol_Append-追加模式
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail
   */
   PROTOCOLSHARED_EXPORT int32_t SetCycReadAddrinfo(std::vector<Protocol::AddrInfoForRW> *addrVec,Protocol::Add_Type addType);

   /**
   *  @brief 绑定的地址值发送变化回调
   *  @param[in] addrVec：地址信息数组 callback：回调函数指针
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail
   */
   PROTOCOLSHARED_EXPORT int32_t SetRegValueChangeCallback(std::vector<Protocol::AddrInfoForRW> *addrVec, Protocol::Add_Type addType);

   /**
   *  @brief  设置地址值
   *  @param[in] addr:需要写入的地址信息和数据信息 value:数据区 type:操作类型，false-异步，true-同步 callback:异步模式下写入完成回调函数
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail
   */
   PROTOCOLSHARED_EXPORT int32_t SetValue(Protocol::AddrInfoForRW *addr, Protocol::Write_Opt_Type WType);

   /**
   *  @brief  读取地址值
   *  @param[in] addr:需要读取的地址信息 valueVec:数据区  iType:操作类型，0-获取当前最新值，1-同步获取地址最新值，2-异步读一次
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail，type-true情况下是通讯读失败，否则就是当前地址没有更新过值
   */
   PROTOCOLSHARED_EXPORT int32_t GetValue(Protocol::AddrInfoForRW *addr, Protocol::Read_Opt_Type RType);

   /**
   *  @brief  获取寄存器列表信息
   *  @param[in] addrList：寄存器列表信息
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail
   */
   PROTOCOLSHARED_EXPORT int32_t GetRegList(std::list<std::string> *addrList);

   /**
   *  @brief  通过寄存器名称获取详细信息
   *  @param[in] info：寄存器信息数据
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail
   */
   PROTOCOLSHARED_EXPORT int32_t GetRegInfo(Protocol::RegInfo *info);

   /**
   *  @brief  设置标签
   *  @param[in] tagList: 标签列表
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail
   */
   PROTOCOLSHARED_EXPORT int32_t SetTagList(const std::vector<Protocol::Tag_Info> *tagList);

   /**
   *  @brief 获取标签
   *  @param[in] tagList: 标签列表
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail
   */
   PROTOCOLSHARED_EXPORT int32_t GetTagList(std::vector<Protocol::Tag_Info> *tagList);

   /**
   *  @brief  检测地址信息是否正确
   *  @param[in] info：地址信息, transforFlag：地址格式转换，false-字符串地址转数值，true-数值地址转字符串
   *  @return:
   *  -- 正确，返回 Protocol_Addr_Normal
   *  -- 寄存器错误，返回 Protocol_Addr_Err_Reg
   *  -- 地址错误，返回 Protocol_Addr_Err_Index
   */
   PROTOCOLSHARED_EXPORT int32_t CheckAddr(Protocol::AddrInfoForRW *info, bool transforFlag);

   /**
   *  @brief  通讯协议版本号
   *  @param[in] sVersion：版本号
   *  @return:
   *  -- 成功，返回 Protocol_Rtn_Success
   *  -- 失败，返回 Protocol_Rtn_Fail
   */
   PROTOCOLSHARED_EXPORT int32_t GetVersion(std::string *sVersion);

}


#endif // PROTOCOL_H
