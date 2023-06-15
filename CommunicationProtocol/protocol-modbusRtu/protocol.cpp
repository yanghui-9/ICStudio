#include <iterator>
#include "protocol.h"
#include "service.h"
#include "modbusRtu.h"

#include "../../communication/communicationdef.h"
#include "../../communication/communicationglobal.h"

/**
*  @brief  获取协议需要被外部配置的参数.
*  @param[in]  config：协议默认参数.
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success
*  -- 失败，返回 Protocol_Rtn_Fail
*/
int32_t GetDeviceConfig(Protocol::Device_Config *config)
{
    Device &D = Device::GetInstance();
    D.m_lock.lock();

    //初始化参数.
    if(!D.m_MDevice)
    {
        D.m_MDevice = std::make_shared<modbusRtu>();
        D.m_MDevice->Init();
    }

    //参数.
    *config = Device::GetInstance().m_deviceConfig;

    D.m_lock.unlock();
    return Protocol_Rtn_Success;
}


/**
*  @brief  初始化协议，支持动态调整参数.
*  @param[in]  comm：通讯接口智能指针 config：协议参数.
*  @return:
*  -- 成功，返回 Protocol_Rtn_Success.
*  -- 失败，返回 Protocol_Rtn_Fail.
*/
int32_t InitDevice(std::shared_ptr<ICommunication> *comm, const Protocol::Device_Config *config)
{
    Device &D = Device::GetInstance();

    //config 指针为null表示关闭线程.
    if(!config)
    {
        D.StopThread();
        return Protocol_Rtn_Success;
    }

    D.m_lock.lock();

    //初始化参数.
    if(!D.m_MDevice)
    {
        D.m_MDevice = std::make_shared<modbusRtu>();
        D.m_MDevice->Init();
    }

    //配置更新.
    D.m_deviceConfig = *config;

    //初始化.
    D.InitDevice(*comm);

    //配置变动处理.
    D.UpdateConfig(*config);

    Device::GetInstance().m_lock.unlock();

    return Protocol_Rtn_Success;
}
