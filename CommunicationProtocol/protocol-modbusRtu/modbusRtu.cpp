#include <map>
#include <ostream>
#include <sstream>
#include <QString>
#include <QStringList>
#include "modbusRtu.h"
#include "service.h"
#include "typedefheader.h"

#include "../../communication/communicationdef.h"
#include "../../communication/communicationglobal.h"

using namespace Protocol;

modbusRtu::modbusRtu()
{
}

void modbusRtu::Init()
{
    Device &D = Device::GetInstance();

    //1.初始化设备参数.
    D.m_deviceConfig.commInfo.commType = Comm_Type_Serial_Port;//通讯类型.
    D.m_deviceConfig.commInfo.isThread = true;//协议内部开线程执行process.
    D.m_deviceConfig.commInfo.isOnline = true;//在线模式.
    D.m_deviceConfig.commInfo.sCommPar = "COM7-9600-8-N-1-10-0"; //通讯参数‘-’间隔.

    D.m_deviceConfig.protocolInfo.station = 1;
    D.m_deviceConfig.protocolInfo.deviceStation = 2;
    D.m_deviceConfig.protocolInfo.timeout = 500;
    D.m_deviceConfig.protocolInfo.communicationInterval = 20;
    D.m_deviceConfig.protocolInfo.retryTimes = 3;
    D.m_deviceConfig.protocolInfo.RWThreshold = 3;//优先级阈值.
    D.m_deviceConfig.protocolInfo.byteOrder16 = AB;//字节序.
    D.m_deviceConfig.protocolInfo.byteOrder32 = ABCD;
    D.m_deviceConfig.protocolInfo.masterOrSlave = Master;//主站从站.
    D.m_deviceConfig.protocolInfo.bitPackageMax = 120*4;   //位最大长度组包.
    D.m_deviceConfig.protocolInfo.wordPackageMax = 120*2;  //字最大长度组包.
    D.m_deviceConfig.protocolInfo.bitPackageInterval = 16*4;//位组包最大间隔长度.
    D.m_deviceConfig.protocolInfo.wordPackageInterval = 16;//字组包最大间隔长度.
    D.m_deviceConfig.protocolInfo.nonstandardParameters = "";//非标准参数.

    //2.初始化特殊配置.
    D.m_bCycReadFrameCache = true;//循环读帧缓存模式.
    D.m_iFrameDataFormat = Device::frame_hex;//帧数据格式.

    //3.初始化寄存器基本信息.
    D.m_regInfoMap.clear();
    D.m_regInfoMap.emplace("0x",RegInfo{"0x","DDDD",65535,1,readWrite,1});
    D.AddCommStatusRegInfo();
}

int32_t modbusRtu::ReadFrame(const std::vector<AddrInfo> &addrInfo)
{
    //初始化.
    static Device &D = Device::GetInstance();
    D.InitFrame(frame);
    if(0 == addrInfo.size())
    {
        return -1;
    }
    AddrInfo vInfo = addrInfo.front();

    //开始组帧.
    D.InitFrame(frame);

    //帧长度
    frame.send.resize(8);
    //站号
    frame.send[0] = static_cast<char>(D.m_deviceConfig.protocolInfo.deviceStation & 0xff);
    //功能码
    if("0x" == vInfo.reg)
    {
        frame.send[1] = 0x01;
    }
    else if("1x" == vInfo.reg)
    {
        frame.send[1] = 0x02;
    }
    else if ("3x" == vInfo.reg) {
        frame.send[1] = 0x04;
    }
    else {
        frame.send[1] = 0x03;
    }
    //地址
    uint64_t addr = vInfo.index;
    frame.send[2] = static_cast<char>( (addr) >> 8 );
    frame.send[3] = static_cast<char>( (addr) );

    //长度
    uint32_t len = vInfo.len;
    frame.send[4] = static_cast<char> (len >> 8);//长度
    frame.send[5] = static_cast<char> (len);

    //校验
    unsigned short crc;//crc校验结果
    crc = cal_crc_(reinterpret_cast<unsigned char *>(frame.GetSendBuff()), 6);//校验
    frame.send[6] = static_cast<char>(crc & 0xff);
    frame.send[7] = static_cast<char>( (crc >> 8) & 0xff);

    //计算偏移量(每个标签对应一个偏移量).
    //槽号.
    //再计算长度.
    frame.sendLen = 8;
    frame.recvLen =  static_cast<int32_t>(6);

    //配置解析参数
    frame.readList.clear();
    vInfo.beginIndex = 3;
    vInfo.bitOffset = 0;
    frame.readList.push_back(vInfo);

    //入队
    D.m_curFrameList.push_back(frame);

    return 1;
}

int32_t modbusRtu::WriteFrame(const std::vector<Protocol::AddrInfoForRW> &addrInfo)
{
    //初始化.
    return 0;
}

int32_t modbusRtu::CheckFrame(Frame &recvF)
{
    //未实现
    //1.长度，校验码，帧序号，帧头帧尾固定字段等等
    if(recvF.recv.size() != static_cast<uint32_t>( recvF.recvLen ) )
    {
        return -1;
    }
    return 0;
}

int32_t modbusRtu::GetVersion(std::string &sVersion)
{
    sVersion = "1.0";//主版本.子版本   接口有增加等重大改变可以修改主版本号.
    return 0;
}
