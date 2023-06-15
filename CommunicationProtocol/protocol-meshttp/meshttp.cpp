#include <map>
#include <ostream>
#include <sstream>
#include <QString>
#include <QStringList>
#include "meshttp.h"
#include "service.h"
#include "typedefheader.h"

#include "../../communication/communicationdef.h"
#include "../../communication/communicationglobal.h"

using namespace Protocol;

mes_http::mes_http()
{
}

mes_http::~mes_http()
{
}

void mes_http::Init()
{
    Device &D = Device::GetInstance();

    //1.初始化设备参数
    D.m_deviceConfig.commInfo.commType = Comm_Type_Tcp_Client;//通讯类型
    D.m_deviceConfig.commInfo.isThread = false;//协议内部开线程执行process
    D.m_deviceConfig.commInfo.isOnline = true;//在线模式
    D.m_deviceConfig.commInfo.sCommPar = "36.154.106.106-50091"; //通讯参数‘-’间隔

    D.m_deviceConfig.protocolInfo.station = 1;
    D.m_deviceConfig.protocolInfo.deviceStation = 2;
    D.m_deviceConfig.protocolInfo.timeout = 5000;
    D.m_deviceConfig.protocolInfo.communicationInterval = 30;
    D.m_deviceConfig.protocolInfo.retryTimes = 3;
    D.m_deviceConfig.protocolInfo.RWThreshold = 3;//优先级阈值
    D.m_deviceConfig.protocolInfo.byteOrder16 = AB;//字节序
    D.m_deviceConfig.protocolInfo.byteOrder32 = ABCD;
    D.m_deviceConfig.protocolInfo.masterOrSlave = Master;//主站从站
    D.m_deviceConfig.protocolInfo.bitPackageMax = 120*4;   //位最大长度组包
    D.m_deviceConfig.protocolInfo.wordPackageMax = 120*2;  //字最大长度组包
    D.m_deviceConfig.protocolInfo.bitPackageInterval = 16*4;//位组包最大间隔长度
    D.m_deviceConfig.protocolInfo.wordPackageInterval = 16;//字组包最大间隔长度
    D.m_deviceConfig.protocolInfo.nonstandardParameters = "";//非标准参数

    //2.初始化特殊配置
    D.m_bCycReadFrameCache = true;//循环读帧缓存模式
    D.m_protocolReadMode = Device::continuousRead;//全连续分块组帧
    D.m_iFrameDataFormat = Device::frame_hex;//帧数据格式

    //3.初始化寄存器基本信息
    D.m_regInfoMap.clear();
    D.m_regInfoMap.emplace("Send",RegInfo{"Send","DDDD",65535,0,WriteOnly,8});
    D.m_regInfoMap.emplace("Recv",RegInfo{"Recv","DDDD",65535,0,readOnly,8});
    D.AddCommStatusRegInfo();
}

int32_t mes_http::ReadFrame(const std::vector<AddrInfo> &addrInfo)
{
    return 0;
}

int32_t mes_http::WriteFrame(const std::vector<Protocol::AddrInfoForRW> &addrInfo)
{
    //初始化
    Device &D = Device::GetInstance();
    D.InitFrame(frame);
    D.m_curFrameList.clear();
    if(0 == addrInfo.size())
    {
        return -1;
    }

    //开始组帧
    //获取发送uri
    //获取发送data
    std::string sValue = std::get<std::string>(addrInfo.front().value);
    std::vector<std::string> parVector = StringSplit(sValue,"#");
    if(2 != parVector.size())
    {
        return 0;
    }


    //帧头部分
    std::stringstream request_stream;
    request_stream << "POST " << parVector.at(0) <<" HTTP/1.0\r\n";
    request_stream << "Host:" << "36.154.106.106"<< "\r\n";
    request_stream << "User-Agent: C/1.0\r\n";
    request_stream << "Content-Type: application/json; charset=utf-8 \r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Content-Length: " << parVector.at(1).size() << "\r\n";
    request_stream << "Connection: close\r\n\r\n";  //NOTE THE Double line feed
    request_stream << parVector.at(1);

    //帧长度，数据复制
    frame.send.resize(request_stream.str().size());
    memcpy(frame.GetSendBuff(),request_stream.str().data(),request_stream.str().size());

    //配置
    frame.writeList.emplace_back(addrInfo.front());
    frame.sendLen = static_cast<uint32_t>( request_stream.str().size() );
    frame.recvLen =  4096;

    //入队
    D.m_curFrameList.push_back(frame);

    return 1;
}

int32_t mes_http::CheckFrame(Frame &recvF)
{
    //1.通用
    if(recvF.actualRecvLen <= 0)
    {
        return Device::error;
    }
    std::string str(recvF.recv.data(),recvF.actualRecvLen);
    std::string sub = str;
    auto pos = str.find("Content-Length: ");
    std::vector<char> buf;
    int content_len = 0;
    if (pos != std::string::npos)
    {
        for (int i = pos + 16; i < str.size(); i++)
        {
            char c = str[i];
            if (c >= '0' && c <= '9')
            {
                buf.push_back(c - '0');
            }
            else
            {
                break;
            }
        }
        if (buf.size() > 0)
        {
            for (int i = buf.size() - 1, len = 1; i >= 0; len *= 10, --i)
            {
                content_len += buf[i] * len;
            }
        }
        sub = str.substr(str.size() - content_len);
    }

    //2.通用2
    QString msg = QString::fromStdString(sub);  //http返回的string是UTF-8格式
    QStringList CaptureDataList = msg.split(QRegExp("(\r\n){2,}"));
    QString CaptureMes;
    CaptureMes.clear();
    if(!CaptureDataList.isEmpty())
    {
        CaptureMes = CaptureDataList.back(); //去掉http头部,获取返回的数据
    }

    //对应解析部分
    if(!CaptureMes.isNull() && CaptureMes != "")
    {
        Device &D = Device::GetInstance();
        std::string sCapture = std::string(CaptureMes.toLocal8Bit().constData());   //QString正常转string
        DataVariant vLen(static_cast<ushort>( sCapture.size() ) );
        D.SetDataFromAddrAndDatatype("Recv",0,ubit16,vLen);

        DataVariant sData(sCapture);
        D.SetDataFromAddrAndDatatype("Recv",2*8,string,sData);
        //D.SetDataFromAddr("Recv",2*8,static_cast<uint32_t>(sCapture.size()*8),sCapture.data());
        return Device::normal;
    }
    return Device::error;
}

int32_t mes_http::DealRecvData(AddrInfo &addrinfo, char *data)
{
    return -1;
}

void mes_http::BeforeCommunication()
{
    Device &D = Device::GetInstance();
    //http每次都重新连接
    D.m_comm->CloseComm();
    D.m_comm->OpenComm();
}

int32_t mes_http::GetVersion(std::string &sVersion)
{
    sVersion = "1.0";//主版本.子版本   接口有增加等重大改变可以修改主版本号
    return 0;
}
