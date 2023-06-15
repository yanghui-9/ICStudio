#include <map>
#include <ostream>
#include <sstream>
#include <QString>
#include <QStringList>
#include "cip.h"
#include "service.h"
#include "typedefheader.h"

#include "../../communication/communicationdef.h"
#include "../../communication/communicationglobal.h"

using namespace Protocol;

cip::cip()
{
}

void cip::Init()
{
    Device &D = Device::GetInstance();

    //1.初始化设备参数.
    D.m_deviceConfig.commInfo.commType = Comm_Type_Tcp_Client;//通讯类型.
    D.m_deviceConfig.commInfo.isThread = false;//协议内部开线程执行process.
    D.m_deviceConfig.commInfo.isOnline = true;//在线模式.
    D.m_deviceConfig.commInfo.sCommPar = "192.168.1.100-44818"; //通讯参数‘-’间隔.

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
    D.m_protocolReadMode = Device::mixedRead;//全连续分块组帧.
    D.m_mixLenMax = 255;//总长最大255.
    D.m_mixBlockNumMax = 6;//随便先设置了最大6个标签.
    D.m_iFrameDataFormat = Device::frame_hex;//帧数据格式.

    //3.初始化寄存器基本信息.
    D.m_regInfoMap.clear();
    D.AddCommStatusRegInfo();
}

int32_t cip::ReadFrame(const std::vector<AddrInfo> &addrInfo)
{
    //初始化.
    static Device &D = Device::GetInstance();
    D.InitFrame(frame);
    if(0 == addrInfo.size())
    {
        return -1;
    }

    //标签个数.
    uint32_t numTag = static_cast<uint32_t>( addrInfo.size() );
    if(1 == numTag)
    {//单标签.
        OneTagReadFrame(addrInfo.front());
        return 1;
    }

    //开始组帧.
    D.InitFrame(frame);
    //帧头.
    FormFrameHead(frame.send,0x2a,0x6f);
    //命令数据.
    //接口句柄00000000代表CIP.
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    //超时.
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);
    //项数.
    frame.send.emplace_back(0x02);
    frame.send.emplace_back(0x00);
    //空地址项.
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    //空地址项长度.
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    //未连接项.
    frame.send.emplace_back(0xB2);
    frame.send.emplace_back(0x00);
    //cip报文长度.
    frame.send.emplace_back(0x18);
    frame.send.emplace_back(0x00);
    //命令.
    frame.send.emplace_back(0x52);
    //请求路径长度.
    frame.send.emplace_back(0x02);
    //默认请求路径.
    frame.send.emplace_back(0x20);
    frame.send.emplace_back(0x06);
    frame.send.emplace_back(0x24);
    frame.send.emplace_back(0x01);
    //默认超时.
    frame.send.emplace_back(0x0A);
    frame.send.emplace_back(0xF0);
    //cip指令长度.
    frame.send.emplace_back(0x20);
    frame.send.emplace_back(0x00);
    //多标签服务代码.
    frame.send.emplace_back(0x0A);
    //请求路径大小.
    frame.send.emplace_back(0x02);
    //请求路径.
    frame.send.emplace_back(0x20);
    frame.send.emplace_back(0x02);
    frame.send.emplace_back(0x24);
    frame.send.emplace_back(0x01);
    //标签数量.
    frame.send.emplace_back(numTag);
    frame.send.emplace_back(0x00);

    //计算偏移量(每个标签对应一个偏移量).
    uint32_t offset0 = 2+numTag*2;//2+标签数量*2.
    for (uint32_t i = 0; i < numTag; ++i)
    {
        frame.send.emplace_back(offset0 & 0xff);
        frame.send.emplace_back((offset0 >> 8) & 0xff);
        uint64_t tagNameLen = addrInfo.at(i).reg.size();
        offset0 += 6 + ((tagNameLen%2 == 0)?tagNameLen:tagNameLen+1);
    }
    //标签.
    for (uint32_t i = 0; i < numTag ; ++i)
    {
        //服务标识.
        frame.send.emplace_back(0x4C);
        //CIP长度多少字.
        frame.send.emplace_back( (addrInfo.at(i).reg.size()+1)/2+1 );
        //扩展符号.
        frame.send.emplace_back(0x91);
        //PLC标签长度多少个字节.
        uint64_t tagNameLen = addrInfo.at(i).reg.size();
        frame.send.emplace_back( tagNameLen );
        //标签内容.
        foreach (auto n, addrInfo.at(i).reg) {
            frame.send.emplace_back(n);
        }
        if(0 != tagNameLen %2 )
        {//补0.
           frame.send.emplace_back(0);
        }
        //读取长度.
        frame.send.emplace_back(0x01);
        frame.send.emplace_back(0x00);
    }

    //槽号.
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);

    //再计算长度.
    uint64_t len0 = frame.send.size() - 24;
    frame.send[2] = static_cast<char>( len0 & 0xff );
    frame.send[3] = static_cast<char>((len0 >> 8) & 0xff);
    len0 = frame.send.size() - 40;
    frame.send[38] = static_cast<char>(len0 & 0xff);
    frame.send[39] = static_cast<char>((len0 >> 8) & 0xff);
    len0 = frame.send.size() - 54;
    frame.send[48] = static_cast<char>(len0 & 0xff);
    frame.send[49] = static_cast<char>((len0 >> 8) & 0xff);

    frame.sendLen = static_cast<uint32_t>( frame.send.size() );
    frame.recvLen = 1024;//有字符串的标签，接收长度不明，设置大一点
    frame.readList = addrInfo;

    D.m_curFrameList.push_back(frame);

    return 1;
}

int32_t cip::WriteFrame(const std::vector<Protocol::AddrInfoForRW> &addrInfo)
{
    //初始化.
    Device &D = Device::GetInstance();
    D.InitFrame(frame);
    if(0 == addrInfo.size())
    {
        return -1;
    }

    //开始组帧.
    //帧头.
    FormFrameHead(frame.send,static_cast<uint32_t>( 40+addrInfo.front().reg.size() ),0x6f);
    //命令数据.
    //接口句柄00000000代表CIP.
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    //超时.
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);
    //项数.
    frame.send.emplace_back(0x02);
    frame.send.emplace_back(0x00);
    //空地址项.
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    //空地址项长度.
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    //未连接项.
    frame.send.emplace_back(0xB2);
    frame.send.emplace_back(0x00);
    //数据长度指后面数据长度（字节）.
    frame.send.emplace_back(0x1C);
    frame.send.emplace_back(0x00);
    //命令.
    frame.send.emplace_back(0x52);
    //请求路径长度.
    frame.send.emplace_back(0x02);
    //默认请求路径.
    frame.send.emplace_back(0x20);
    frame.send.emplace_back(0x06);
    frame.send.emplace_back(0x24);
    frame.send.emplace_back(0x01);
    //默认超时.
    frame.send.emplace_back(0x0A);
    frame.send.emplace_back(0xF0);
    //长度（从服务标识开始到写入的值结束）.
    uint64_t tagNameLen = addrInfo.front().reg.size();
    tagNameLen = (tagNameLen%2 == 0)?tagNameLen:tagNameLen+1;
    if(Protocol::string != addrInfo.front().dataType)
    {
        frame.send.emplace_back(10+tagNameLen);
        frame.send.emplace_back(0x00);
    }
    else
    {
        frame.send.emplace_back(10+2+tagNameLen);
        frame.send.emplace_back(0x00);
    }
    //服务标识.
    frame.send.emplace_back(0x4D);
    //CIP长度多少字.
    frame.send.emplace_back((addrInfo.front().reg.size()+1)/2+1);
    //固定.
    frame.send.emplace_back(0x91);
    //PLC标签长度多少个字节.
    frame.send.emplace_back(addrInfo.front().reg.size());
    //标签内容.
    foreach (auto n, addrInfo.front().reg) {
        frame.send.emplace_back(n);
    }
    if(addrInfo.front().reg.size() != tagNameLen)
    {//补0.
        frame.send.emplace_back(0);
    }
    //数据类型.
    char command =  GetCommandFromDataType(addrInfo.front().dataType);
    frame.send.emplace_back(command);
    frame.send.emplace_back(0x00);
    //写入数量.
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);
    //写入的值.
    if(Protocol::string == addrInfo.front().dataType)
    {//如果数据类型是字符串.
        //字符串内容.
        if(11 == addrInfo.front().value.index())
        {
            std::string text = std::get<std::string>(addrInfo.front().value);
            //字符串长度.
            uint64_t sLen = text.size();
            frame.send.emplace_back(sLen&0xff);
            frame.send.emplace_back((sLen>>8)&0xff);
            foreach (auto c, text) {
              frame.send.emplace_back(c);
            }
            if(0 != sLen%2)
            {//补0.
              frame.send.emplace_back(0);
            }
        }
    }
    else
    {
        char data[8] = {0};
        D.Variant2Char(addrInfo.front().value,data);
        uint32_t dataLen = GetLenFromDataType(static_cast<ushort>(command&0xff));
        for (uint32_t i = 0; i < dataLen && i < 8; ++i) {
           frame.send.emplace_back(data[i]);
        }
    }
    //槽号.
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);

    //再计算长度.
    RecalculateFrameLen(frame);

    frame.sendLen = static_cast<uint32_t>( frame.send.size() );
    frame.recvLen = 24+20;
    frame.writeList.emplace_back(addrInfo.front());

    D.m_curFrameList.push_back(frame);

    return 1;
}

int32_t cip::CheckFrame(Frame &recvF)
{
    static Device &D = Device::GetInstance();
    //检查是否需要重新握手
    if(24 == recvF.actualRecvLen && recvF.recv.at(8) == 0x64)
    {//无效句柄需要握手
        m_bHandshake = false;
        GetGlobalFrame();
        return Device::error;
    }
    if(!m_bHandshake)
    {//握手.
        if(recvF.actualRecvLen != recvF.recvLen)
        {
            return Device::error;
        }
        //获取plc会话句柄.
        memcpy(m_SessionHandle,recvF.recv.data()+4,4);
        //处理后续帧.
        for (uint32_t index = 1; index < D.m_curFrameList.size(); ++index)
        {
            if(D.m_curFrameList.at(index).sendLen > 8)
            {
                D.m_curFrameList.at(index).send.data()[4] = m_SessionHandle[0];
                D.m_curFrameList.at(index).send.data()[5] = m_SessionHandle[1];
                D.m_curFrameList.at(index).send.data()[6] = m_SessionHandle[2];
                D.m_curFrameList.at(index).send.data()[7] = m_SessionHandle[3];
            }
        }
        m_bHandshake = true;
        D.ClearCycReadCacheFrame();
        return Device::normal;
    }
    else if(Device::write == D.m_optFlag)
    {//写入.
        if(recvF.actualRecvLen != recvF.recvLen)
        {
            return Device::error;
        }
        return Device::normal;
    }
    else
    {//读.
        if(1 < recvF.readList.size() && 44 < recvF.recv.size() )
        {//多标签.
            uint64_t cipLen = recvF.recv.size() - 44;
            //先判断标签个数.
            uint64_t tagNum =  recvF.readList.size();
            uint64_t tagNum1 = recvF.recv.at(44) + (recvF.recv.at(45) << 8);
            if(tagNum == tagNum1 && cipLen > (1+tagNum)*2 )
            {//标签数量匹配.
                uint64_t offset = 0;
                uint32_t dataLen = 0;
                for (uint32_t num = 0; num < tagNum && (47+num*2) < recvF.recv.size() ;++num)
                {
                   offset =  recvF.recv.at(46+num*2) + (recvF.recv.at(47+num*2) << 8);
                   if(recvF.recv.size() >= (44+offset+8))
                   {
                       dataLen =  GetLenFromDataType( recvF.recv.at(44+offset+4) & 0xff);
                       if(0 == dataLen)
                       {//字符串.
                           dataLen =  recvF.recv.at(44+offset+6)+(recvF.recv.at(44+offset+7)<<8);
                           //recvF.recvDataBeginIndex.emplace_back(44+offset+8);
                           recvF.readList.at(num).beginIndex = 44+offset+8;
                           //字符串长度设置.
                           //D.SetDataFromAddrAndDatatype(recvF.readList.at(num).reg,0,Protocol::ubit16,static_cast<ushort>( dataLen ));
                           SetTagOfStringLen(recvF.readList.at(num).reg,dataLen);
                       }
                       else
                       {
                           //recvF.recvDataBeginIndex.emplace_back(44+offset+6);
                           recvF.readList.at(num).beginIndex = 44+offset+6;
                       }
                       //recvF.recvDataLen.emplace_back(dataLen*8);
                       recvF.readList.at(num).dataLen= dataLen*8;
                   }
                }
            }
            else
            {//标签不匹配
              return  Device::error;
            }
            return Device::normal;
        }
        else if(1 == recvF.readList.size() && 48 <=  recvF.recv.size())
        {
            uint32_t dataLen = 0;
            dataLen =  GetLenFromDataType( recvF.recv.at(40+4) & 0xff );
            if(0 == dataLen)
            {//字符串.
                dataLen =  recvF.recv.at(40+6)+(recvF.recv.at(40+7)<<8);
                //recvF.recvDataBeginIndex.emplace_back(40+8);
                recvF.readList.front().beginIndex = 40+8;
                //字符串长度设置.
                //D.SetDataFromAddrAndDatatype(recvF.readList.at(0).reg,0,Protocol::ubit16,static_cast<ushort>( dataLen ));
                SetTagOfStringLen(recvF.readList.at(0).reg,dataLen);
            }
            else
            {
                //recvF.recvDataBeginIndex.emplace_back(40+6);
                recvF.readList.front().beginIndex = 40+6;
            }
            //recvF.recvDataLen.emplace_back(dataLen*8);
            recvF.readList.front().dataLen = dataLen*8;
            return Device::normal;
        }
    }
    return Device::error;
}

int32_t cip::GetVersion(std::string &sVersion)
{
    sVersion = "1.0";//主版本.子版本   接口有增加等重大改变可以修改主版本号.
    return 0;
}

void cip::GetGlobalFrame()
{
    if(!m_bHandshake)
    {//组建握手帧.
        Device &D = Device::GetInstance();
        D.InitFrame(frame);

        FormFrameHead(frame.send,0x04,0x65);
        //协议版本2.
        frame.send.emplace_back(0x01);
        frame.send.emplace_back(0x00);
        //选项标记2.
        frame.send.emplace_back(0x00);
        frame.send.emplace_back(0x00);

        frame.sendLen = static_cast<uint32_t>( frame.send.size() );
        frame.recvLen = 28;
        frame.readList.clear();
        frame.bGlobalFrame = true;
        D.m_curFrameList.push_front(frame);
    }
}

void cip::FormFrameHead(std::vector<char> &send,uint32_t sendLen,char fun)
{
    //注册请求命令2.
    send.emplace_back(fun);
    send.emplace_back(0x00);
    //长度2.
    send.emplace_back(sendLen & 0xff);
    send.emplace_back(0x00);
    //会话句柄4.
    send.emplace_back(m_SessionHandle[0]);
    send.emplace_back(m_SessionHandle[1]);
    send.emplace_back(m_SessionHandle[2]);
    send.emplace_back(m_SessionHandle[3]);
    //状态4.
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    //描述8.
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    //默认选项4.
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    send.emplace_back(0x00);
    send.emplace_back(0x00);
}

uint32_t cip::GetLenFromDataType(unsigned short datatype)
{
    uint32_t len = 0;
    switch (datatype) {
    case 0xc1:
    {//bool
       len = 2;
    }
        break;
    case 0xc2:
    {//sint
       len = 2;
    }
        break;
    case 0xc3:
    {//short
       len = 2;
    }
        break;
    case 0xc4:
    {//int
       len = 4;
    }
        break;
    case 0xc7:
    {//ushort
       len = 2;
    }
        break;
    case 0xc8:
    {//uint
       len = 4;
    }
        break;
    case 0xca:
    {//float
       len = 4;
    }
        break;
    case 0xcb:
    {//double
       len = 8;
    }
        break;
    case 0xd0:
    {//string
       len = 0;
    }
        break;
    }
    return len;
}

char cip::GetCommandFromDataType(Protocol_DataType dataType)
{
    char command;
    switch (dataType) {
    case bit:
    {
       command = static_cast<char>(0xc1);
    }
        break;
    case bit8:
    {
       command = static_cast<char>(0xc2);
    }
        break;
    case ubit16:
    {
        command = static_cast<char>(0xc7);
    }
        break;
    case bit16:
    {
        command = static_cast<char>(0xc3);
    }
        break;
    case bit32:
    {
        command = static_cast<char>(0xc4);
    }
        break;
    case ubit32:
    {
        command = static_cast<char>(0xc8);
    }
        break;
    case float32:
    {
        command = static_cast<char>(0xca);
    }
        break;
    case double64:
    {
        command = static_cast<char>(0xcb);
    }
        break;
    case string:
    {
        command = static_cast<char>(0xd0);
    }
        break;
    default:
    {
        command = static_cast<char>(0xc3);
    }
    }
    return command;
}

void cip::OneTagReadFrame(const AddrInfo &addrInfo)
{
    //初始化.
    static Device &D = Device::GetInstance();

    //开始组帧.
    D.InitFrame(frame);
    //帧头.
    FormFrameHead(frame.send,0x2a,0x6f);
    //命令数据.
    //接口句柄00000000代表CIP.
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    //超时.
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);
    //项数.
    frame.send.emplace_back(0x02);
    frame.send.emplace_back(0x00);
    //空地址项.
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    //空地址项长度.
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x00);
    //未连接项.
    frame.send.emplace_back(0xB2);
    frame.send.emplace_back(0x00);
    //cip报文长度.
    frame.send.emplace_back(0x18);
    frame.send.emplace_back(0x00);
    //命令.
    frame.send.emplace_back(0x52);
    //请求路径长度.
    frame.send.emplace_back(0x02);
    //默认请求路径.
    frame.send.emplace_back(0x20);
    frame.send.emplace_back(0x06);
    frame.send.emplace_back(0x24);
    frame.send.emplace_back(0x01);
    //默认超时.
    frame.send.emplace_back(0x0A);
    frame.send.emplace_back(0xF0);
    //cip指令长度.
    uint64_t tagNameLen = addrInfo.reg.size();
    tagNameLen = (tagNameLen%2 == 0)?tagNameLen:tagNameLen+1;
    frame.send.emplace_back(6+tagNameLen);
    frame.send.emplace_back(0x00);

    //标签.
    //服务标识.
    frame.send.emplace_back(0x4C);
    //CIP长度多少字.
    frame.send.emplace_back((addrInfo.reg.size()+1)/2+1);
    //扩展符号.
    frame.send.emplace_back(0x91);
    //PLC标签长度多少个字节.
    frame.send.emplace_back( addrInfo.reg.size() );
    //标签内容.
    foreach (auto n, addrInfo.reg) {
        frame.send.emplace_back(n);
    }
    if(addrInfo.reg.size() != tagNameLen)
    {//补0.
        frame.send.emplace_back(0);
    }
    //读取长度.
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);


    //槽号.
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);
    frame.send.emplace_back(0x01);
    frame.send.emplace_back(0x00);

    //再计算长度.
    RecalculateFrameLen(frame);

    frame.sendLen = static_cast<uint32_t>( frame.send.size() );
    frame.recvLen = 1024;//有字符串的标签，接收长度不明，设置大一点.
    frame.readList.push_back(addrInfo);

    //frame.RWCallback = addrInfo.varList.front().callback;

    D.m_curFrameList.push_back(frame);
}

//len 字节长度.
void cip::SetTagOfStringLen(const std::string &name, uint64_t len)
{
    static Device &D = Device::GetInstance();
    std::map<std::string,Protocol::RegInfo>::iterator it = D.m_regInfoMap.find(name);
    if(it != D.m_regInfoMap.end())
    {
        it->second.unitBitNum = len *8;
    }
    else
    {
        Protocol::RegInfo reginfo;
        reginfo.reg = name;
        reginfo.unitBitNum = len *8;
        reginfo.dataType = Protocol::string;

        D.m_lock.lock();
        D.m_regInfoMap[name] = reginfo;
        D.m_lock.unlock();
    }
}

void cip::RecalculateFrameLen(Frame &curframe)
{
    uint64_t len0 = curframe.send.size() - 24;
    curframe.send[2] = static_cast<char>( len0 & 0xff );
    curframe.send[3] = static_cast<char>((len0 >> 8) & 0xff);
    len0 = curframe.send.size() - 40;
    curframe.send[38] = static_cast<char>(len0 & 0xff);
    curframe.send[39] = static_cast<char>((len0 >> 8) & 0xff);
}
