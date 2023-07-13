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
#include "service_def.h"
#include "ValueChangeDeal.h"
#include "DataAreaDeal.h"

class ICommunication;
class Protocol_Interface;
class Device
{
public:
    enum Type
    {
        cycleRead = 0,  //循环读.
        conditionRead,  //条件读.
        write          //写入.
    };

    enum Read_Type
    {
        continuousRead = 0,//连续读.
        mixedRead          //混合读.
    };

    enum Frame_Data_Type
    {
        frame_hex = 0, //帧格式16进制.
        frame_asc,      //asc帧格式.
        frame_asc_sData  //asc帧格式，按数据类型转
    };

    enum Check_Reslut_Type  //解析数据返回结果类型.
    {
        normal = 0, //正常.
        error = 1,  //数据错误.
        retry = 2   //重试，再次接收.
    };

    static Device &GetInstance();

    //获取地址区信息.
    int32_t GetRefInfoFromReg(const std::string &reg, Protocol::RegInfo &info);
    //获取寄存器地址单位（位）
    int32_t GetRegUint(const std::string &reg);

    //获取组包参数.
    int32_t GetPackageParameters(std::string &reg, int32_t &interval, int32_t &max);

    //根据不同的通信协议初始化相关参数.
    void InitDevice(std::shared_ptr<ICommunication> &comm);

    //更新配置动作.
    void UpdateConfig(const Protocol::Device_Config &config);

    //std::variant2char*
    void Variant2Char(const Protocol::DataVariant &dataV, char * value);
    void Char2Variant(Protocol::Protocol_DataType dataType, std::vector<char> &valueV, Protocol::DataVariant &data);
    static void StringToCharOfDataType(Protocol::Protocol_DataType dataType,const std::string &sData, char *data);

    //地址转换.
    int32_t AddrS2D(const std::string &reg,const std::string &sIndex,uint64_t &index);
    int32_t AddrD2S(const std::string &reg,const uint64_t &index,std::string &sIndex);

    //设置地址变化回调信息.
    int32_t SetRegValueChangeCallback(std::vector<Protocol::AddrInfoForRW> &addrVec,
                                      Protocol::Add_Type addType);

    //地址优化分块.
    void MergeAddr(Type type);

    //解析接收数据.
    int32_t AnalysisFrame(Frame &recvF);

    //一轮通讯结束结果处理.
    void DealCommunicationFinish(int32_t optFlag, const Frame &curFrame , int32_t result);

    //获取地址缓存中的数据.
    int32_t GetLastData(Protocol::AddrInfoForRW &addr);
    //设置地址缓存中的数据.
    int32_t SetLastData(Protocol::AddrInfoForRW &addr);

    //获取地址缓存中的数据.
    int32_t GetDataFromAddrAndDatatype(const std::string &reg, const uint64_t &index, const Protocol::Protocol_DataType &dataType, Protocol::DataVariant &data);
    //设置地址缓存中的数据.
    int32_t SetDataFromAddrAndDatatype(const std::string &reg, const uint64_t &index, const Protocol::Protocol_DataType &dataType, const Protocol::DataVariant &data);

    //清空循环读帧缓存.
    void ClearCycReadCacheFrame();

    //版本号.
    int32_t GetVersion(std::string &sVersion);

    //process master
    int32_t Process(std::vector<Protocol::AddrInfoForRW> &addrList);
    //process Slave
    int32_t Process_Slave();
    //process all
    int32_t Process_All();

    //Thread
    void Run();
    void StopThread();

    //标签处理转换.
    int32_t TagDeal(Protocol::AddrInfoForRW &addr);

    //通过数据类型获取位数.
    void GetBitNumFromDatatype(const std::string &reg, Protocol::Protocol_DataType dataType, uint64_t &bitNum);

    //初始化Frame.
    void InitFrame(Frame &frame);

    //添加连通讯状态-寄存器信息.
    void AddCommStatusRegInfo();

    //更新通讯结果到状态寄存器.
    void UpdataProcessResultToDataArea(int32_t ret,const std::vector<Protocol::AddrInfoForRW> &addrList);

protected:
    //获取解析索引
    uint64_t GetBeginIndex(const Protocol::AddrInfoForRW &addrRw, const AddrInfo &addr);
    //获取解析位偏移
    uint64_t GetBitOffset (const Protocol::AddrInfoForRW &addrRw, const AddrInfo &addr);

public:
    ~Device();

private:
    //开始处理接收收据处理.
    int32_t RecvData();
    //构造.
    Device();
    //组建发送帧.
    int32_t BuildSendFrame(std::vector<Protocol::AddrInfoForRW> &addrVec);
    //从站模式下数据回调.
    int Slave_ReadReady_CB(int code, char *buf, uint32_t len,char **sBuf, uint32_t *sLen);


private:
    //处理数据区
    DataAreaDeal m_DataAreaDeal;

public:
    std::shared_ptr<ICommunication> m_comm;
    Protocol::Device_Config m_deviceConfig;

    //循环读.
    std::vector<AddrInfo> m_cycReadVector;
    //混合读取分段索引(混合块开始索引).
    std::vector<uint32_t> m_cycMixedRead;
    //指向当前读索引.
    uint32_t m_curReadIndex;
    //缓存的循环读取帧.
    std::map<uint32_t,std::deque<Frame> > m_cycFrameMapOfCurReadIndex;
    //循环读帧是否缓存，不用重新生成.
    bool m_bCycReadFrameCache = true;
    //通讯协议读取模式设定.
    uint32_t m_protocolReadMode = continuousRead;
    //循环读锁.
    std::mutex m_cycReadLock;

    //条件读.
    std::deque<Protocol::AddrInfoForRW> m_conditonReadList;
    //连续条件读次数.
    int32_t m_conditonReadTimes = 0;
    //条件读锁.
    std::mutex m_conditionReadLock;

    //写入.
    std::deque<Protocol::AddrInfoForRW> m_writeList;
    //连续写入次数.
    int32_t m_writeTimes = 0;
    //立即写入锁.
    std::mutex m_writeLock;

    //标签别名对应信息.
    std::map<std::string,Protocol::Tag_Info> m_tagInfoMap;

    //当前操作模式，写入，读，条件读等等.
    int32_t m_optFlag;

    //寄存器基本信息.
    std::map<std::string,Protocol::RegInfo> m_regInfoMap;

    //解析接收帧数据部分格式，asc或者hex.
    int32_t m_iFrameDataFormat = frame_hex;

    //当前发送帧列表.
    std::deque<Frame> m_curFrameList;

    //地址缓冲区.
    std::set<Data_Area> m_dataArea;
    std::mutex m_dataAreaLock;

    //同步读写标识.
    int32_t m_syncRWFlag = 0;

    //初始化标识.
    bool m_bInifFlag = false;

    //设备基本信息读写互斥锁.
    std::mutex m_lock;

    //通讯锁
    std::mutex m_commLock;

    //线程指针（线程模式下）.
    std::shared_ptr<std::thread> m_processThread;
    bool m_bThreadFlag = true;
    bool m_threadIsRun = false;
    bool m_threadIsOver = false;

    //连续通讯错误次数.
    uint32_t m_errTimes = 0;

    //全混合分块-最大块数.
    int32_t m_mixBlockNumMax = -1;

    //全混合分块-最大总长.
    int32_t m_mixLenMax = -1;

    //实现.
    std::shared_ptr<Protocol_Interface> m_MDevice;
};

#endif // device_H
