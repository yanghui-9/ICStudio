#include <iterator>
#include "protocol.h"
#include "service.h"

#include "../../communication/communicationdef.h"
#include "../../communication/communicationglobal.h"

/**
*  @brief 设置循环读取地址信息，地址整合优化分块并返回优化分块结果
*  @param[in] addrVec：输入地址信息，输入优化分块结果  addType：操作类型，Protocol_Cover-覆盖模式 Protocol_Append-追加模式
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success
*  -- 失败，返回 Protocol_Rtn_Fail
*/
PROTOCOLSHARED_EXPORT int32_t SetCycReadAddrinfo(std::vector<Protocol::AddrInfoForRW> &addrVec, Protocol::Add_Type addType)
{
    Device &D = Device::GetInstance();

    if(Protocol::Slave == D.m_deviceConfig.protocolInfo.masterOrSlave)
    {//从站模式下不支持
        return Protocol_Rtn_Fail;
    }
    std::lock_guard lockGuard(D.m_cycReadLock);

    //标签别名转换
    foreach (auto info, addrVec) {
        D.TagDeal(info);
    }

    //地址信息插入
    switch (addType) {
    case Protocol::Append:
    {
        //D.m_cycReadVector.insert(D.m_cycReadVector.end(),addrVec.begin(),addrVec.end());
        for (std::vector<Protocol::AddrInfoForRW>::iterator it = addrVec.begin(); it != addrVec.end(); ++it) {
           D.m_cycReadVector.push_back(D.AddrInfoForRW2AddrInfo(*it));
        }
    }
        break;
    default:
    {
        //D.m_cycReadVector.assign(addrVec.begin(),addrVec.end());
        D.m_cycReadVector.clear();
        for (std::vector<Protocol::AddrInfoForRW>::iterator it = addrVec.begin(); it != addrVec.end(); ++it) {
           D.m_cycReadVector.push_back(D.AddrInfoForRW2AddrInfo(*it));
        }
    }
        break;
    }

    //地址对齐
    for (auto var : D.m_cycReadVector) {
       D.DealIndexAlignOfByteForRead(var.index,var.len);
    }

    //是否地址优化分块
    if(D.m_deviceConfig.protocolInfo.isAddrOptimize)
    {
        D.MergeAddr(Device::cycleRead);
        //优化分块之后返回分块结果
        //addrVec.assign(D.m_cycReadVector.begin(),D.m_cycReadVector.end());
    }
    D.m_cycFrameMapOfCurReadIndex.clear();
    D.m_curReadIndex = 0;

    return Protocol_Rtn_Success;
}

/**
*  @brief 绑定的地址值发送变化回调
*  @param[in] addrVec：地址信息数组 callback：回调函数指针
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success
*  -- 失败，返回 Protocol_Rtn_Fail
*/
PROTOCOLSHARED_EXPORT int32_t SetRegValueChangeCallback(const std::vector<Protocol::AddrInfoForRW> &addrVec,
                                                        Protocol::Add_Type addType , const Protocol::DataCallback &callback)
{
    //地址信息插入
    Device &D = Device::GetInstance();
    std::vector<Protocol::AddrInfo> addrRWVec;
    foreach (auto addr, addrVec) {
      addrRWVec.push_back(D.AddrInfoForRW2AddrInfo(addr));
    }
    D.SetRegValueChangeCallback(addrRWVec,addType,callback);
    return Protocol_Rtn_Success;
}

/**
*  @brief  通讯一次，包含循环读取+异步写入+条件读取，默认写入优先级>条件读取>循环读取，优先级阈值默认为3（例如连续写入三次之后必须条件读一次）
*  @param
*  @return
*  -- 没有需要操作的事务，返回 Protocol_Process_Null
*  -- 读取成功，返回  Protocol_Process_R_Suc
*  -- 读取失败，返回  Protocol_Process_R_NoData
*  -- 读取失败，返回  Protocol_Process_R_CheckError
*  -- 写入成功，返回  Protocol_Process_W_Suc
*  -- 写入失败，返回  Protocol_Process_W_NoData
*  -- 写入失败，返回  Protocol_Process_W_CheckError
*/
PROTOCOLSHARED_EXPORT int32_t Process()
{
    static Device &D = Device::GetInstance();
    return  D.Process_All();
}

/**
*  @brief  设置地址值
*  @param[in] addr:需要写入的地址信息和数据信息  type:操作类型，false-异步，true-同步
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success
*  -- 失败，返回 Protocol_Rtn_Fail
*/
PROTOCOLSHARED_EXPORT int32_t SetValue(Protocol::AddrInfoForRW &addr, const Protocol::Write_Opt_Type &WType)
{
    int ret = 0;
    ret = Protocol_Rtn_Success;
    Device &D = Device::GetInstance();
    Protocol::Write_Opt_Type OptType = WType;

    //标签转换处理
    D.TagDeal(addr);

    //加入队列
    if( Protocol::Master == D.m_deviceConfig.protocolInfo.masterOrSlave && D.m_deviceConfig.commInfo.isOnline)
    {//主站模式下且在线模式
        //入队，有相同的写入则更新，避免重复
        D.m_writeLock.lock();
        //第一个不做比较处理，可能正在操作中
        static std::deque<Protocol::AddrInfoForRW>::iterator itFind;
        if( 1 < D.m_writeList.size() )
        {
            itFind = std::find(D.m_writeList.begin()+1,D.m_writeList.end(),addr);
            if(itFind != D.m_writeList.end())
            {//如果已经存在
                itFind->valueVec = addr.valueVec;
            }
            else
            {
                D.m_writeList.push_back(addr);
            }
        }
        else
        {
            D.m_writeList.push_back(addr);
        }
        D.m_writeLock.unlock();
    }
    else
    {//从站模式或者离线模式直接取消同步写入
        OptType = Protocol::AsynWrite;
    }


    //值更新到缓存中
    ret = D.SetLastData(addr);

    //通讯
    if(Protocol::SyncWrite == OptType)
    {//同步设置值
        D.m_lock.lock();
        D.m_syncRWFlag = Device::write;
        std::vector<Protocol::AddrInfo> addrListOut;
        if(Protocol_Process_W_Suc != D.Process(addrListOut))
        {
            ret = Protocol_Rtn_Fail;
        }
        D.m_syncRWFlag = 0;
        D.m_lock.unlock();
    }

    return ret;
}

/**
*  @brief  读取地址值
*  @param[in] addr:需要读取的地址信息 valueVec:数据区  iType:操作类型，0-获取当前最新值，1-同步获取地址最新值，2-异步读一次成功回调
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success
*  -- 失败，返回 Protocol_Rtn_Fail，type-true情况下是通讯读失败，否则就是当前地址没有更新过值
*/
PROTOCOLSHARED_EXPORT int32_t GetValue(Protocol::AddrInfoForRW &addr,const Protocol::Read_Opt_Type &RType)
{
    int ret = 0;
    ret = Protocol_Rtn_Success;
    Device &D = Device::GetInstance();

    //标签转换处理
    D.TagDeal(addr);

    //分类处理
    switch (RType) {
    case 0:
    {//获取当前读到了最新值
        ret = D.GetLastData(addr);
    }
        break;
    case 1:
    case 2:
    {//同步设备读
        //从站和离线模式不支持同步异步
        if( Protocol::Slave == D.m_deviceConfig.protocolInfo.masterOrSlave || !D.m_deviceConfig.commInfo.isOnline)
        {
            return Protocol_Rtn_Fail;
        }

        //判断是否同步
        if(Protocol::SyncRead == RType)
        {//同步
          D.m_lock.lock();
        }

        //加入条件读列表
        D.m_conditionReadLock.lock();
        //第一个不做比较处理，可能正在操作中
        static std::deque<Protocol::AddrInfoForRW>::iterator itFind;
        if( 1 < D.m_conditonReadList.size() )
        {
            itFind = std::find(D.m_conditonReadList.begin()+1,D.m_conditonReadList.end(),addr);
            if(itFind == D.m_conditonReadList.end())
            {//如果不存在
               D.m_conditonReadList.push_back(addr);
            }
        }
        else
        {
           D.m_conditonReadList.push_back(addr);
        }
        D.m_conditionReadLock.unlock();

        if(Protocol::SyncRead == RType)
        {//开始同步操作
            D.m_syncRWFlag = Device::conditionRead;
            std::vector<Protocol::AddrInfo> addrListOut;
            //if(Protocol_Process_Condition_Suc != Process(addrListOut))
            if(0 != D.Process(addrListOut))
            {
                //ret = Protocol_Rtn_Fail;
                ret = -1;
            }
            else
            {//同步读成功获取当前最新值
                ret = D.GetLastData(addr);
            }
            D.m_syncRWFlag = 0;
        }

        //判断是否同步
        if(Protocol::SyncRead == RType)
        {//同步
            D.m_lock.unlock();
        }
    }
        break;
    }

    return ret;
}


/**
*  @brief  获取寄存器列表信息
*  @param[in] addrList：寄存器列表信息
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success
*  -- 失败，返回 Protocol_Rtn_Fail
*/
PROTOCOLSHARED_EXPORT int32_t GetRegList(std::list<std::string> &addrList)
{
    Device &D = Device::GetInstance();
    std::lock_guard lockGuard(D.m_lock);

    std::transform(D.m_regInfoMap.begin(), D.m_regInfoMap.end(), std::inserter(addrList, addrList.end()),
                   [](std::pair<const std::string,Protocol::RegInfo> &pair){ return pair.first; });
    return Protocol_Rtn_Success;
}

/**
*  @brief  通过寄存器名称获取详细信息
*  @param[in] info：寄存器信息数据
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success
*  -- 失败，返回 Protocol_Rtn_Fail
*/
PROTOCOLSHARED_EXPORT int32_t GetRegInfo(Protocol::RegInfo &info)
{
    Device &D = Device::GetInstance();
    std::lock_guard lockGuard(D.m_lock);
    return D.GetRefInfoFromReg(info.reg,info);
}

/**
*  @brief  注册标签
*  @param[in] tagList: 标签列表
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success
*  -- 失败，返回 Protocol_Rtn_Fail
*/
PROTOCOLSHARED_EXPORT int32_t SetTagList(const std::vector<Protocol::Tag_Info> &tagList)
{
    Device &D = Device::GetInstance();
    std::lock_guard lockGuard(D.m_lock);

#ifdef TAG_MODE
    //协议标签协议格式
    D.m_regInfoMap.clear();
    foreach (auto info, tagList) {
        D.m_regInfoMap.emplace(info.name,Protocol::RegInfo{info.name,info.rwType,info.dataType});
    }
#else
    //标签别名模式，注册别名
    D.m_tagInfoMap.clear();
    foreach (auto info, tagList) {
        D.m_tagInfoMap.insert(std::pair<std::string,Protocol::Tag_Info>(info.name,info));
    }
#endif
    return Protocol_Rtn_Success;
}

/**
*  @brief  获取当前标签列表
*  @param[in] tagList: 标签列表
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success
*  -- 失败，返回 Protocol_Rtn_Fail
*/
PROTOCOLSHARED_EXPORT int32_t GetTagList(std::vector<Protocol::Tag_Info> &tagList)
{
    Device &D = Device::GetInstance();
    std::lock_guard lockGuard(D.m_lock);
#ifdef TAG_MODE
    //协议标签格式
    foreach (auto info, D.m_regInfoMap) {
        tagList.emplace_back(info.second.reg,info.second.rwFlag,info.second.dataType);
    }
#else
    //标签别名模式
    foreach (auto info, D.m_tagInfoMap) {
        tagList.push_back(info.second);
    }
#endif
    return Protocol_Rtn_Success;
}


/**
*  @brief  检测地址信息是否正确
*  @param[in] info：地址信息, transforFlag：地址格式转换，false-字符串地址转数值，true-数值地址转字符串
*  @return:
*  -- 正确，返回 Protocol_Addr_Normal
*  -- 寄存器错误，返回 Protocol_Addr_Err_Reg
*  -- 地址错误，返回 Protocol_Addr_Err_Index
*/
PROTOCOLSHARED_EXPORT int32_t CheckAddr(Protocol::AddrInfo &info, bool transforFlag)
{
    Device &D = Device::GetInstance();

    std::lock_guard lockGuard(D.m_lock);
    std::map<std::string,Protocol::RegInfo>::iterator it = D.m_regInfoMap.find(info.reg);
    if(it == D.m_regInfoMap.end())
    {
        return Protocol_Addr_Err_Reg;
    }

    if(!transforFlag)
    {
        if(-1 ==  D.AddrS2D(info.reg,info.sIndex,info.index) )
        {
            return Protocol_Addr_Err_Index;
        }
    }
    else
    {
        if(-1 ==  D.AddrD2S(info.reg,info.index,info.sIndex) )
        {
            return Protocol_Addr_Err_Index;
        }
    }

    return Protocol_Addr_Normal;
}


/**
*  @brief  通讯协议版本号
*  @param[in] sVersion：版本号
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success
*  -- 失败，返回 Protocol_Rtn_Fail
*/
int32_t GetVersion(std::string &sVersion)
{
    return Device::GetInstance().GetVersion(sVersion);
}
