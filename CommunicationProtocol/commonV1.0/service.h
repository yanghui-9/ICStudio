#ifndef device_H
#define device_H
#include <list>
#include <functional>
#include <set>
#include <deque>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include "protocol_type.h"
//#include <QDebug>

class ICommunication;

#define ADDRESS_AREA_UNIT (2048)
#define RECV_BUFF_MAX     (10240)

//收发帧结构
struct Frame
{
    std::vector<char> send;
    std::vector<char> recv;

    uint32_t sendLen = 0;//发送长度
    int32_t recvLen = 0;//是预计接收长度
    int32_t actualRecvLen = 0;//实际接收长度

    std::vector<Protocol::AddrInfo> readList;//读取地址列表
    std::vector<uint32_t> recvDataBeginIndex;//接收数据，解析数据开始索引
    std::vector<uint32_t> recvDataLen;//解析数据的长度

    Protocol::DataWCallback RWCallback = nullptr;//写入完成回调

    char * GetSendBuff(int len = -1){
        if(-1 != len)
        {
            send.resize(static_cast<uint32_t>(len));
        }
        if(0 != len)
            return const_cast<char *>( send.data() );
        else
            return nullptr;
    }
    char * GetRecvBuff(int len = -1){
        if(-1 != len)
        {
            recv.resize(static_cast<uint32_t>(len));
        }
        if(0 != len)
        {
            return const_cast<char *>( recv.data() );
        }
        else {
            return nullptr;
        }
    }
};

struct Data_Area
{
    std::string reg;
    uint64_t    index;
    char        data[ADDRESS_AREA_UNIT] = {0};
    bool operator < (const Data_Area & info) const
    {
        int ret = strcmp(reg.data(),info.reg.data());
        if(0 > ret){
            return true;
        }else if (0 == ret) {
            //if( abs( static_cast<int64_t>(info.index) - static_cast<int64_t>(index) ) >= (ADDRESS_AREA_UNIT*8) )
            if( index/(ADDRESS_AREA_UNIT*8) <info.index/(ADDRESS_AREA_UNIT*8) )
            {
                return true;
            }
        }
        return false;
    }
};

//地址变化绑定
struct Addr_Change_Data
{
    //绑定地址值变化
    Protocol::AddrInfo valueChangeAddrInfo;
    //存上一次值，地址回调时用来比较是否变化
    std::vector<char>  valueChangeAddrInfoBuff;
    std::vector<char>  valueChangeAddrInfoBuffNew;

    bool operator < (const Addr_Change_Data & info) const
    {
        return valueChangeAddrInfo < info.valueChangeAddrInfo;
    }

    bool operator == (const Addr_Change_Data & info) const
    {
        return valueChangeAddrInfo == info.valueChangeAddrInfo;
    }

    Addr_Change_Data(const Protocol::AddrInfo &var)
    {
      valueChangeAddrInfo = var;
      uint64_t size = var.len%8 == 0?var.len/8:var.len/8+1;
      valueChangeAddrInfoBuff.resize(size);
      memset(const_cast<char *>(valueChangeAddrInfoBuff.data()),0,size);
      valueChangeAddrInfoBuffNew.resize(size);
      memset(const_cast<char *>(valueChangeAddrInfoBuffNew.data()),0,size);
    }
};

class Protocol_Interface;
class Device
{
public:
    enum Type
    {
        cycleRead = 0,  //循环读
        conditionRead,  //条件读
        write          //写入
    };

    enum Read_Type
    {
        continuousRead = 0,//连续读
        mixedRead          //混合读
    };

    enum Frame_Data_Type
    {
        frame_hex = 0, //帧格式16进制
        frame_asc      //asc帧格式
    };

    enum Check_Reslut_Type  //解析数据返回结果类型
    {
        normal = 0, //正常
        error = 1,  //数据错误
        retry = 2   //重试，再次接收
    };

    static Device &GetInstance();

    //获取地址区信息
    int32_t GetRefInfoFromReg(const std::string &reg, Protocol::RegInfo &info);

    //获取组包参数
    int32_t GetPackageParameters(std::string &reg, int32_t &interval, int32_t &max);

    //根据不同的通信协议初始化相关参数
    void InitDevice(std::shared_ptr<ICommunication> &comm);

    //更新配置动作
    void UpdateConfig(const Protocol::Device_Config &config);

    //std::variant2char*
    void Variant2Char(const Protocol::DataVariant &dataV, char * value);
    void Char2Variant(Protocol::Protocol_DataType dataType,char * value, Protocol::DataVariant &data);

    //地址转换
    int32_t AddrS2D(const std::string &reg,const std::string &sIndex,uint64_t &index);
    int32_t AddrD2S(const std::string &reg,const uint64_t &index,std::string &sIndex);

    //处理地址值变化回调
    void DealAddrValueChangeCB(const Protocol::AddrInfo &addrInfo, int32_t ret);
    //设置地址变化回调信息
    int32_t SetRegValueChangeCallback(const std::vector<Protocol::AddrInfo> &addrVec,
                                      Protocol::Add_Type addType , const Protocol::DataCallback &callback);

    //地址优化分块
    void MergeAddr(Type type);

    //解析接收数据
    int32_t AnalysisFrame(Frame &recvF);

    //一轮通讯结束结果处理
    void DealCommunicationFinish(int32_t optFlag, const Protocol::DataWCallback &RWCallback , int32_t result);

    //组建发送帧
    int32_t BuildSendFrame(std::vector<Protocol::AddrInfo> &addrVec);

    //处理读地址信息中的地址索引字节对齐
    void DealIndexAlignOfByteForRead(uint64_t &index, uint64_t &len);

    //同步操作
    int32_t SyncProcess();

    //地址数据结构转换
    Protocol::AddrInfo AddrInfoForRW2AddrInfo(const Protocol::AddrInfoForRW &addrForRW);

    //获取地址缓存中的数据
    int32_t GetLastData(Protocol::AddrInfoForRW &addr);
    //设置地址缓存中的数据
    int32_t SetLastData(const Protocol::AddrInfoForRW &addr);

    //获取地址缓存中的数据
    int32_t GetDataFromAddrAndDatatype(const std::string &reg, const uint64_t &index, const Protocol::Protocol_DataType &dataType, Protocol::DataVariant &data);
    //设置地址缓存中的数据
    int32_t SetDataFromAddrAndDatatype(const std::string &reg, const uint64_t &index, const Protocol::Protocol_DataType &dataType, const Protocol::DataVariant &data);

    //获取地址缓存中的数据
    int32_t GetDataFromAddr(const std::string &reg, uint64_t index,uint32_t len,char * data);
    //设置地址缓存数据
    int32_t SetDataFromAddr(const std::string &reg, uint64_t index,uint32_t len,char * data);

    //版本号
    int32_t GetVersion(std::string &sVersion);

    //process
    int32_t Process(std::vector<Protocol::AddrInfo> &addrList);
    //process Slave
    int32_t Process_Slave();
    //process all
    int32_t Process_All();

    //Thread
    void Run();
    void StopThread();

    //从站模式下数据回调
    int Slave_ReadReady_CB(int code, char *buf, uint32_t len,char **sBuf, uint32_t *sLen);

    //标签处理转换
    int32_t TagDeal(Protocol::AddrInfoForRW &addr);
    //标签处理转换2
    int32_t TagDeal(Protocol::AddrInfo &addr);

    //通过数据类型获取位数
    void GetBitNumFromDatatype(Protocol::Protocol_DataType dataType,uint32_t &bitNum);

    //获取协议字符串数据类型的个数和长度
    inline void GetTypeStringOfNumAndLen(uint64_t in_num, uint64_t &out_num, uint32_t &out_len);

    //初始化Frame
    void InitFrame(Frame &frame);

public:
    ~Device();

private:
    //开始处理接收收据处理
    int32_t RecvData();

private:
    Device();

    //获取地址块的迭代器，不存在则插入地址块
    std::set<Data_Area>::iterator GetDataAreaIterator(Data_Area &dataA);


public:
    std::shared_ptr<ICommunication> m_comm;
    Protocol::Device_Config m_deviceConfig;

    //循环读
    std::vector<Protocol::AddrInfo> m_cycReadVector;
    //混合读取分段索引
    std::vector<uint32_t> m_cycMixedRead;
    //指向当前读索引
    uint32_t m_curReadIndex;
    //缓存的循环读取帧
    std::map<uint32_t,std::deque<Frame> > m_cycFrameMapOfCurReadIndex;
    //循环读帧是否缓存，不用重新生成
    bool m_bCycReadFrameCache = true;
    //通讯协议读取模式设定
    uint32_t m_protocolReadMode = continuousRead;
    //循环读锁
    std::mutex m_cycReadLock;

    //条件读
    std::deque<Protocol::AddrInfoForRW> m_conditonReadList;
    //连续条件读次数
    int32_t m_conditonReadTimes = 0;
    //条件读锁
    std::mutex m_conditionReadLock;

    //写入
    std::deque<Protocol::AddrInfoForRW> m_writeList;
    //连续写入次数
    int32_t m_writeTimes = 0;
    //立即写入锁
    std::mutex m_writeLock;

    //值变化回调
    Protocol::DataCallback m_valueChangeCallback;
    //绑定地址值变化
    //std::vector<Protocol::AddrInfo> m_valueChangeAddrInfo;
    //存上一次值，地址回调时用来比较是否变化
    //std::vector<std::vector<char> > m_valueChangeAddrInfoBuff;
    //std::vector<std::vector<char> > m_valueChangeAddrInfoBuffNew;
#ifndef TAG_MODE
    //非协议标签模式
    std::vector<Addr_Change_Data> m_valueChangeAddrInfo;
#else
    std::set<Addr_Change_Data> m_valueChangeAddrInfo;
#endif

    //标签别名对应信息
    std::map<std::string,Protocol::Tag_Info> m_tagInfoMap;

    //值变化信息锁
    std::mutex m_valueChangeLock;

    //当前操作模式，写入，读，条件读等等
    int32_t m_optFlag;

    //寄存器基本信息
    std::map<std::string,Protocol::RegInfo> m_regInfoMap;

    //解析接收帧数据部分格式，asc或者hex
    int32_t m_iFrameDataFormat = frame_hex;

    //当前发送帧列表
    std::deque<Frame> m_curFrameList;

    //地址缓冲区
    std::set<Data_Area> m_dataArea;
    std::mutex m_dataAreaLock;

    //同步读写标识
    int32_t m_syncRWFlag = 0;

    //初始化标识
    bool m_bInifFlag = false;

    //设备基本信息读写互斥锁
    std::mutex m_lock;

    //线程指针（线程模式下）
    std::shared_ptr<std::thread> m_processThread;
    bool m_bThreadFlag = true;
    bool m_threadIsRun = false;
    bool m_threadIsOver = false;

    //连续通讯错误次数
    uint32_t m_errTimes = 0;

    //全混合分块-最大块数
    int32_t m_mixBlockNumMax = -1;

    //全混合分块-最大总长
    int32_t m_mixLenMax = -1;

    //实现
    std::shared_ptr<Protocol_Interface> m_MDevice;
};

#endif // device_H
