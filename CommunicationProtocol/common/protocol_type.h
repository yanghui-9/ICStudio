#ifndef PROTOCOLTYPE_H
#define PROTOCOLTYPE_H
#include <string>
#include <variant>
#include <vector>
#include <functional>

//返回值定义.
#define Protocol_Process_Null (0xffff)
#define Protocol_Process_R_Suc (0)
#define Protocol_Process_R_NoData (1)
#define Protocol_Process_R_CheckError (2)
#define Protocol_Process_W_Suc (3)
#define Protocol_Process_W_NoData (4)
#define Protocol_Process_W_CheckError (5)
#define Protocol_Process_W_DataTypeError (6)


#define Protocol_Rtn_Success (0)
#define Protocol_Rtn_Fail   (-1)

#define Protocol_Addr_Normal    (0)
#define Protocol_Addr_Err_Reg   (1)
#define Protocol_Addr_Err_Index (2)

#define DataVariant_Null (0)

#define Protocol_Status_Reg_Name "status"

class ICommunication;

namespace Protocol {

struct AddrInfoForRW;
struct Device_Config;
struct RegInfo;
using DataVariant = std::variant<std::monostate,bool,char,unsigned char,unsigned short,short,uint32_t,int32_t,float,uint64_t,int64_t,double,std::string>;
using DataVector = std::vector<DataVariant>;
using DataWCallback = std::function<void (const AddrInfoForRW &,int32_t)>;


//数据类型.
enum Protocol_DataType
{
    bit = 1,
    ubit8,
    bit8,
    ubit16,
    bit16,
    ubit32,
    bit32,
    float32,
    ubit64,
    bit64,
    double64,
    string,
    null_dType = 0xffff
};

//操作类型.
enum Read_Opt_Type
{
    //当前值.
    CurrentValue = 0,
    //同步.
    SyncRead,
    //异步.
    AsynRead
};

enum Write_Opt_Type
{
    //异步.
    AsynWrite = 0,
    //同步.
    SyncWrite
};

//通讯接口类型.
enum Comm_Type
{
    Comm_Type_Serial_Port = 0,
    Comm_Type_Tcp_Client,
    Comm_Type_Tcp_Server,
    Comm_Type_Udp,
    Comm_Type_Max = 0xff
};

//16 32字节序.
enum Byte_Order_16
{
    AB = 0,
    BA
};

//字节序.
enum Byte_Order_32
{
    ABCD = 0,
    BADC,
    CDAB,
    DCBA
};

//主站从站.
enum Master_Slave_Mode
{
    Master = 0, //主站.
    Slave,      //从站.
    Mix         //混合模式.
};

//增加类型.
enum Add_Type
{
    Cover = 0,//覆盖.
    Append    //追加.
};

//读写类型
enum RW_Type
{
    readOnly = 0,//只读.
    WriteOnly,   //只写.
    readWrite    //读写.
};


//协议相关参数.
struct ProtocolInfo
{
    bool isAddrOptimize = true; //地址合并优化.
    uint32_t timeout;   //通信超时时间(ms).
    uint32_t retryTimes;//重试次数.
    uint32_t communicationInterval; //通信间隔时间(ms).

    //注意：字位组包默认参数需要自己根据协议计算.
    //例如modbus-rtu，两个地址块如果合并可以节约发送大概13个字节，算上cpu响应时间默认字组包最大间隔可以16.
    uint32_t bitPackageInterval;//位组包最大间隔（位单位）.
    uint32_t wordPackageInterval;//字组包最大间隔（字节单位）.
    uint32_t bitPackageMax;//位组包最大（位单位）.
    uint32_t wordPackageMax;//字组包最大（字节单位）.

    int32_t station;       //本机站号.
    int32_t deviceStation; //设备站号.
    int32_t masterOrSlave;  //主从站 0 - 主站 1 - 从站.
    int32_t byteOrder16;//16位字节顺序 0-AB 1-BA.
    int32_t byteOrder32;//32位字节顺序 0-ABCD 1-BADC 2-CDAB 3-DCBA.
    int32_t RWThreshold;//读写优先级阈值，一般默认为3.
    std::string nonstandardParameters;//非标准协议参数，plcl类型选择等等.
};

//通信相关参数.
struct CommInfo
{
    Comm_Type commType = Comm_Type_Max;
    std::string sCommPar;//协议默认通信参数 COM7-9600-8-E-10-0 串口分别对应串口-波特率-数据位-校验位-停止位-串口轮询时间-流控（0-无流控，1-硬件流控，2-软件流控）.
                         //192.168.1.100-502.
    bool isThread = false; //true-表示通信协议内部自开线程处理，false-异步操作需要外部调用Process接口.
    bool isOnline = true; //在线离线模式，false-离线模式，true-在线模式.
    bool isOpenLog = false; //通讯日志，false-关闭通讯日志，true-打开通讯日志.
    bool isOpenErrLog = true; //通讯日志，false-关闭通讯异常日志，true-打开通讯异常日志.
    bool isAddrOptimize = true; //地址合并优化.
};

//设备参数
struct Device_Config
{
    CommInfo commInfo;//通信相关参数.
    ProtocolInfo protocolInfo;//协议相关参数.
};

//读写带数据类型的地址信息.
struct AddrInfoForRW
{
    std::string linkName;      //存在多个连接时，表示连接id.
    std::string reg;           //地址模式下-寄存器，标签模式下-标签名称.
    std::string sIndex;        //显示地址.
    uint64_t index = 0;        //地址模式下-地址索引，协议标签模式下无效.
    Protocol_DataType dataType;//数据类型.
    uint64_t len = 0;          //自动转换处理无需填写.
    DataWCallback callback = nullptr;//回调.
    Protocol::DataVariant value;//值.

    AddrInfoForRW(){}
    AddrInfoForRW(const std::string &_linkName,const std::string &_reg,uint64_t _index):linkName(_linkName),reg(_reg),index(_index){}

#ifndef TAG_MODE
    //非协议标签模式.
    bool operator == (const AddrInfoForRW & info) const
    {
        if(reg == info.reg && index == info.index)
        {
            return true;
        }
        return false;
    }
    bool operator < (const AddrInfoForRW & info) const
    {
        if(reg < info.reg)
        {
            return true;
        }
        else if(reg == info.reg)
        {
            if(index < info.index)
            {
                return true;
            }
        }
        return false;
    }
#else
    bool operator == (const AddrInfoForRW & info) const
    {
        if(reg == info.reg)
        {
            return true;
        }
        return false;
    }
    bool operator < (const AddrInfoForRW & info) const
    {
        if(reg < info.reg)
        {
            return true;
        }
        return false;
    }
#endif
};

//标签.
struct Tag_Info
{
    std::string name;          //标签名称.
    Protocol_DataType dataType = bit32;//数据类型.
    RW_Type rwType = readWrite;//标签读写类型.

    AddrInfoForRW addrInfo; //标签绑定的地址.

    bool operator < (const Tag_Info & info) const
    {
        if(name < info.name)
            return true;
        return false;
    }

    bool operator == (const Tag_Info & info) const
    {
        if(name == info.name)
        {
            return true;
        }
        return false;
    }

    Tag_Info(const std::string &_name,
             const RW_Type &_rwType,
             const Protocol_DataType &_dataType):name(_name),dataType(_dataType),rwType(_rwType){}
    Tag_Info(){}
};

//寄存器信息.
struct RegInfo
{
    std::string reg;   //地址模式下-寄存器，标签模式下-标签名称.
    std::string format;//地址格式.
    uint64_t indexMin;//地址最小值.
    uint64_t indexMax;//地址最大值.
    RW_Type  rwFlag;      //读写类型.
    uint32_t unitBitNum; //地址协议模式下：一个整数地址单位对应位的个数，协议标签模式下：表示字符串类型的长度（位）.
    Protocol_DataType dataType;//地址模式下-默认数据类型，标签模式下-标记数据类型.
    RegInfo(const std::string r,const std::string f,uint64_t iMax,uint64_t iMin,RW_Type rwF,uint32_t uBitNum)
        :reg(r),format(f),indexMin(iMin),indexMax(iMax),rwFlag(rwF),unitBitNum(uBitNum),dataType(ubit16){}
    RegInfo(const std::string r,RW_Type rwF,Protocol_DataType dType)
        :reg(r),indexMin(0),indexMax(0),rwFlag(rwF),unitBitNum(0),dataType(dType){}
    RegInfo(){}
};

using P_GetDeviceConfig = int32_t (*) (Protocol::Device_Config *config);
using P_InitDevice = int32_t (*) (std::shared_ptr<ICommunication> *comm,const Protocol::Device_Config *config);
using P_Process = int32_t (*) ();
using P_SetCycReadAddrinfo = int32_t (*)(std::vector<Protocol::AddrInfoForRW> *addrVec,Protocol::Add_Type);
using P_SetRegValueChangeCallback = int32_t (*)(std::vector<Protocol::AddrInfoForRW> *addrVec,Protocol::Add_Type);
using P_SetValue =  int32_t (*)(Protocol::AddrInfoForRW *, Protocol::Write_Opt_Type );
using P_GetValue = int32_t (*)(Protocol::AddrInfoForRW *, Protocol::Read_Opt_Type );
using P_GetRegList = int32_t (*)(std::list<std::string> *addrList);
using P_GetRegInfo = int32_t (*)(Protocol::RegInfo *info);
using P_SetTagList = int32_t (*)(const std::vector<Protocol::Tag_Info> *);
using P_GetTagList = int32_t (*)(std::vector<Protocol::Tag_Info> *);
using P_CheckAddr = int32_t (*)(Protocol::AddrInfoForRW *info,bool);
using P_GetVersion = int32_t (*)(std::string *);

//协议插件库导出接口函数指针定义
struct P_Protocol
{//以下函数指针定义对应协议插件接口
   P_GetDeviceConfig GetDeviceConfig = nullptr;
   P_InitDevice InitDevice = nullptr;
   P_Process  Process  = nullptr;
   P_SetCycReadAddrinfo SetCycReadAddrinfo = nullptr;
   P_SetRegValueChangeCallback  SetRegValueChangeCallback = nullptr;
   P_SetValue SetValue = nullptr;
   P_GetValue GetValue = nullptr;
   P_GetRegList GetRegList  = nullptr;
   P_GetRegInfo GetRegInfo = nullptr;
   P_SetTagList SetTagList = nullptr;
   P_GetTagList GetTagList = nullptr;
   P_CheckAddr CheckAddr = nullptr;
   P_GetVersion GetVersion = nullptr;
};

}

#endif // PROTOCOLTYPE_H
