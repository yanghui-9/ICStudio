#include <QString>
#include <QStringList>
//#include <QDebug>

#include "service.h"
#include "typedefheader.h"
#include "protocol_interface.h"

#include "../../communication/communicationdef.h"
#include "../../communication/communicationglobal.h"

using namespace Protocol;

Device::Device()
{
    //
}

Device::~Device()
{
    StopThread();
}

int32_t Device::RecvData()
{
    //开始接收数据
    static char recvBuff[RECV_BUFF_MAX] = {0};
    int ret = m_comm->RecvData(recvBuff,m_curFrameList.front().recvLen,
                             m_deviceConfig.protocolInfo.timeout);
    m_curFrameList.front().actualRecvLen = ret;
    if(ret < 0)
    {//重连
        m_comm.get()->CloseComm();
        m_comm.get()->OpenComm();
    }
    else if (ret > 0)
    {
       memcpy(m_curFrameList.front().GetRecvBuff(ret),recvBuff,static_cast<uint32_t>(ret));
    }
    ret = AnalysisFrame(m_curFrameList.front());
    switch (ret) {
    case error:
    {//校验或者解析失败
        if(m_errTimes++ >= m_deviceConfig.protocolInfo.retryTimes)
        {//达到重试次数
            m_errTimes = 0;
            m_curFrameList.pop_front();
            switch (m_optFlag) {
            case cycleRead:
            case conditionRead:
            {
                if(m_curFrameList.front().actualRecvLen > 0)
                {//有数据接收到
                    ret = Protocol_Process_R_CheckError;
                }
                else
                {
                    ret = Protocol_Process_R_NoData;
                }
            }
                break;
            case write:
            {
                if(m_curFrameList.front().actualRecvLen > 0)
                {//有数据接收到
                    ret = Protocol_Process_W_CheckError;
                }
                else
                {
                    ret = Protocol_Process_W_NoData;
                }
            }
                break;
            }
        }
    }
        break;
    case retry:
    {//再读
       ret = RecvData();
    }
        break;
    default:
    {
        //通讯正常完成，出
        m_curFrameList.pop_front();
        switch (m_optFlag) {
        case cycleRead:
        case conditionRead:
        {
            ret = Protocol_Process_R_Suc;
        }
            break;
        case write:
        {
            ret = Protocol_Process_W_Suc;
        }
            break;
        }
    }
    }

    //返回结果
    return ret;
}

std::set<Data_Area>::iterator Device::GetDataAreaIterator(Data_Area &dataA)
{
    std::set<Data_Area>::iterator it =  m_dataArea.find(dataA);
    if(it == m_dataArea.end())
    {
        //先初始化地址块再插入
        dataA.index = ( dataA.index/(ADDRESS_AREA_UNIT*8) )*(ADDRESS_AREA_UNIT*8);
        memset(dataA.data,0,ADDRESS_AREA_UNIT);
        m_dataAreaLock.lock();//插入锁
        auto pair =  m_dataArea.insert(dataA);
        m_dataAreaLock.unlock();
        if(!pair.second)
        {
            return m_dataArea.end();
        }
        it = pair.first;
    }
    return it;
}

Device &Device::GetInstance()
{
    static Device driver;
    return driver;
}

int32_t Device::GetRefInfoFromReg(const std::string &reg,RegInfo &info)
{
    std::map<std::string,Protocol::RegInfo>::iterator it = m_regInfoMap.find(reg);
    if(it != m_regInfoMap.end())
    {
        info = it->second;
        return Protocol_Rtn_Success;
    }
    return Protocol_Rtn_Fail;
}

int32_t Device::GetPackageParameters(std::string &reg,int32_t &interval, int32_t &max)
{
    RegInfo info;
    if(-1 == GetRefInfoFromReg(reg,info))
    {
        return -1;
    }
    else
    {
        if(1 == info.unitBitNum)
        {//位
            interval = static_cast<int32_t>( Device::GetInstance().m_deviceConfig.protocolInfo.bitPackageInterval );
            max = static_cast<int32_t>( Device::GetInstance().m_deviceConfig.protocolInfo.bitPackageMax );
        }
        else
        {
            interval = static_cast<int32_t>( Device::GetInstance().m_deviceConfig.protocolInfo.wordPackageInterval*8 );
            max = static_cast<int32_t>( Device::GetInstance().m_deviceConfig.protocolInfo.wordPackageMax*8 );
        }
    }
    return 0;
}

void Device::InitDevice(std::shared_ptr<ICommunication> &comm)
{
    //初始化通讯接口
    if(comm)
    {
        m_comm = comm;
    }
    else
    {
        if (!m_comm)
        {
            std::shared_ptr<ICommunication> ptrComm(CreateComm(static_cast<ICommunication::Communication_Type>( m_deviceConfig.commInfo.commType ),
                                                               m_deviceConfig.commInfo.sCommPar));
            if(ptrComm)
            {
                ptrComm->OpenComm();
                m_comm = ptrComm;
            }
        }
        comm = m_comm;
    }

    //Tcp服务端回调初始化
    m_comm->OpenComm(0,500,nullptr,nullptr,
                     std::bind(&Device::Slave_ReadReady_CB,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,
                                                     std::placeholders::_4,std::placeholders::_5));
}

void Device::UpdateConfig(const Device_Config &config)
{
    //判断线程模式
    if(config.commInfo.isThread)
    {//线程process
        if(m_processThread)
        {//线程已经创建
            m_threadIsRun = true;
        }
        else
        {
            m_threadIsRun = true;
            m_processThread = std::make_shared<std::thread>(&Device::Run,this);
        }
    }
    else
    {//非线程模式
        if(m_processThread)
        {//线程已经创建，进入暂停模式
            m_threadIsRun = true;
        }
    }

    //判断通讯日志开启
    if(config.commInfo.isOpenLog)
    {
        if(m_comm)
            m_comm->OpenLog("comm_",1);
    }
    else
    {
        if(m_comm)
            m_comm->CloseLog();
    }
}

void Device::Variant2Char(const DataVariant &dataV, char *value)
{
    switch (dataV.index()) {
    case 0:
    {//char
        value[0] = std::get<char>(dataV);
    }
        break;
    case 1:
    {//uchar
        value[0] = static_cast<char>( std::get<unsigned char>(dataV) );
    }
        break;
    case 2:
    {//short
        short s;
        s = std::get<short>(dataV) ;
        memcpy(value,&s,2);
        if(m_MDevice)
        {
            m_MDevice->DealByteOrder(value,bit16);
        }
    }
        break;
    case 3:
    {//ushort
        unsigned short us;
        us = std::get<unsigned short>(dataV) ;
        memcpy(value,&us,2);
        if(m_MDevice)
        {
            m_MDevice->DealByteOrder(value,ubit16);
        }
    }
        break;
    case 4:
    {//uint
        uint32_t ui;
        ui = std::get<uint32_t>(dataV) ;
        memcpy(value,&ui,4);
        if(m_MDevice)
        {
            m_MDevice->DealByteOrder(value,ubit32);
        }
    }
        break;
    case 5:
    {//int
        int32_t i;
        i = std::get<int32_t>(dataV) ;
        memcpy(value,&i,4);
        if(m_MDevice)
        {
            m_MDevice->DealByteOrder(value,bit32);
        }
    }
        break;
    case 6:
    {//float
        float i;
        i = std::get<float>(dataV) ;
        memcpy(value,&i,4);
        if(m_MDevice)
        {
            m_MDevice->DealByteOrder(value,bit32);
        }
    }
        break;
    case 7:
    {//uint64
        uint64_t i;
        i = std::get<uint64_t>(dataV) ;
        memcpy(value,&i,8);
        if(m_MDevice)
        {
            //m_MDevice->DealByteOrder(value,bit32);
        }
    }
        break;
    case 8:
    {//int64
        int64_t i;
        i = std::get<int64_t>(dataV) ;
        memcpy(value,&i,8);
        if(m_MDevice)
        {
            //m_MDevice->DealByteOrder(value,bit32);
        }
    }
        break;
    case 9:
    {//double
        double i;
        i = std::get<double>(dataV) ;
        memcpy(value,&i,8);
        if(m_MDevice)
        {
            //m_MDevice->DealByteOrder(value,bit32);
        }
    }
        break;
    }
}

void Device::Char2Variant(Protocol_DataType dataType,char *value, DataVariant &data)
{
    //开始数据转换
    switch (dataType) {
    case Protocol::bit:
    {
        data = value[0];
    }
        break;
    case Protocol::bit8:
    {
        data = value[0];
    }
        break;
    case Protocol::ubit8:
    {
        data = static_cast<unsigned char>(value[0]);
    }
        break;
    case Protocol::ubit16:
    {
        data = *(static_cast<unsigned short *>(static_cast<void *>(value)));
    }
        break;
    case Protocol::bit16:
    {
        data = *(static_cast<short *>(static_cast<void *>(value)));
    }
        break;
    case Protocol::ubit32:
    {
        data = *(static_cast<uint32_t *>(static_cast<void *>(value)));
    }
        break;
    case Protocol::bit32:
    {
        data = *(static_cast<int32_t *>(static_cast<void *>(value)));
    }
        break;
    case Protocol::float32:
    {
        data = *(static_cast<float *>(static_cast<void *>(value)));
    }
        break;
    case Protocol::ubit64:
    {
        data = *(static_cast<uint64_t *>(static_cast<void *>(value)));
    }
        break;
    case Protocol::bit64:
    {
        data = *(static_cast<int64_t *>(static_cast<void *>(value)));
    }
        break;
    case Protocol::double64:
    {
        data = *(static_cast<double *>(static_cast<void *>(value)));
    }
        break;
    default:
    {
        //
    }
    }
}

int32_t Device::AddrS2D(const std::string &reg, const std::string &sIndex, uint64_t &index)
{
    if(m_MDevice)
    {
        return m_MDevice->AddrS2D(reg,sIndex,index);
    }
    return -1;
}

int32_t Device::AddrD2S(const std::string &reg, const uint64_t &index, std::string &sIndex)
{
    if(m_MDevice)
    {
        return m_MDevice->AddrD2S(reg,index,sIndex);
    }
    return  -1;
}

int32_t Device::SetRegValueChangeCallback(const std::vector<Protocol::AddrInfo> &addrVec,
                                  Protocol::Add_Type addType , const Protocol::DataCallback &callback)
{
    m_valueChangeLock.lock();

    //标签别名转换
    foreach (auto info, addrVec) {
        TagDeal( info );
    }

    //添加类型判断
    if(Protocol::Cover == addType)
    {
        m_valueChangeAddrInfo.clear();
    }

    //添加
    foreach (auto var, addrVec) {
#ifndef TAG_MODE
       m_valueChangeAddrInfo.emplace_back(var);
#else
       //协议标签模式
       m_valueChangeAddrInfo.emplace(var);
#endif
    }

#ifndef TAG_MODE
    //非协议标签模式
    //先排序
    //std::sort(m_valueChangeAddrInfo.begin(),m_valueChangeAddrInfo.end());
    //再去重
    m_valueChangeAddrInfo.erase( std::unique(m_valueChangeAddrInfo.begin(), m_valueChangeAddrInfo.end() ),
                                   m_valueChangeAddrInfo.end());
#endif

    m_valueChangeLock.unlock();
    m_valueChangeCallback = callback;
    return 0;
}

void Device::DealAddrValueChangeCB(const AddrInfo &addrInfo,int32_t ret)
{
#ifndef TAG_MODE
    //非协议标签模式
    if(!m_valueChangeCallback)
    {
        return;
    }
    //处理值变化回调
    m_valueChangeLock.lock();
    Addr_Change_Data ACD(addrInfo);
    std::vector<Addr_Change_Data>::iterator it;
    it = m_valueChangeAddrInfo.begin();
    while (it != m_valueChangeAddrInfo.end())
    {//比较，重载了==，有交叉的地址会相等
        //效率后续优化，自己写个二分查找
        it = std::find(it,m_valueChangeAddrInfo.end(),ACD);
        //it = std::binary_search(it,m_valueChangeAddrInfo.end(),addrInfo);//二分查找
        if(it != m_valueChangeAddrInfo.end())
        {//找到一项地址交叉
            uint32_t dataLen = (it->valueChangeAddrInfo.len%8 == 0?it->valueChangeAddrInfo.len/8:it->valueChangeAddrInfo.len/8+1);//算出字节长度
            char * datanow = const_cast<char*>( it->valueChangeAddrInfoBuffNew.data());
            char * data = const_cast<char*>( it->valueChangeAddrInfoBuff.data());
            GetDataFromAddr(it->valueChangeAddrInfo.reg,it->valueChangeAddrInfo.index,it->valueChangeAddrInfo.len,datanow);
            if(0 != Strcmp(data,datanow,dataLen))
            {//不等
                //qDebug()<<"read:"<<QString::fromStdString(addrInfo.reg)<<addrInfo.index/16<<addrInfo.len/16;
                //qDebug()<<timesss<<QString::fromStdString( it->reg )<<it->index/16<<it->len/16<<dataLen<<(int)data[0]<<(int)data[1]<<(int)datanow[0]<<(int)datanow[1];
                memcpy(data,datanow,dataLen);
                m_valueChangeCallback(it->valueChangeAddrInfo,ret);
            }
            ++it;
        }
    }
    m_valueChangeLock.unlock();
#else
    //协议标签模式
    if(!m_valueChangeCallback)
    {
        return;
    }
    //处理值变化回调
    m_valueChangeLock.lock();
    Addr_Change_Data ACD(addrInfo);
    std::set<Addr_Change_Data>::iterator it;
    it = m_valueChangeAddrInfo.find(ACD);
    if(it != m_valueChangeAddrInfo.end())
    {//存在该标签
        m_valueChangeCallback(it->valueChangeAddrInfo,ret);
    }
    m_valueChangeLock.unlock();
#endif
}

void Device::MergeAddr(Type type)
{
    //Not implemented at present
    if( cycleRead == type)
    {//循环读地址分块
        if(0 == m_cycReadVector.size())
        {
            return;
        }
        std::list<Protocol::AddrInfo> addrList;
        addrList.assign(m_cycReadVector.begin(),m_cycReadVector.end());
        m_curReadIndex = 0;
        m_cycMixedRead.clear();

#if 0
        for (auto var : addrList) {
            qDebug()<<"begin:"<<QString::fromStdString( var.reg )<<var.index<<var.len;
        }
#endif
        //1.先排序
        //std::sort(addrList.begin(),addrList.end());
        addrList.sort();

        //2.去重+合并
        static int32_t intervallen = -1;
        static int32_t maxLen = -1;
        static std::list<Protocol::AddrInfo>::iterator itSecnod;
#if 0
        for (auto var : addrList) {
            qDebug()<<"end1:"<<QString::fromStdString( var.reg )<<var.index<<var.len;
        }
#endif
        for (std::list<Protocol::AddrInfo>::iterator itFirst = addrList.begin(); itFirst != addrList.end();)
        {
            itSecnod = itFirst;
            itSecnod++;
            if(itSecnod == addrList.end())
            {
                break;
            }

            //获取最大间隔和最大长度
            if(-1 == intervallen)
            {//未给过值
                if(-1 == GetPackageParameters(itFirst->reg,intervallen,maxLen))
                {
                   addrList.erase(itFirst++);
                   continue;
                }
            }
            if(itFirst->reg != itSecnod->reg)
            {
                if(-1 == GetPackageParameters(itSecnod->reg,intervallen,maxLen))
                {
                    addrList.erase(itSecnod);
                    itFirst++;
                    continue;
                }
            }

            //判断是否合并
            if(m_MDevice && m_MDevice->JudgeAddrBlockMerge(*itFirst,*itSecnod,intervallen,maxLen))
            {//合并到第一个了，删除第二个
               addrList.erase(itSecnod++);
            }
            else
            {
               ++itFirst;
            }
        }
#if 0
        for (auto var : addrList) {
            qDebug()<<"end2:"<<QString::fromStdString( var.reg )<<var.index<<var.len;
        }
#endif
        //放入循环读取列表
        m_cycReadVector.assign(addrList.begin(),addrList.end());

        //开始混合分块处理
        if(mixedRead == m_protocolReadMode && -1 != m_mixLenMax && -1 != m_mixBlockNumMax)
        {//开始全混合分块
            static uint32_t totalVarNum;
            static uint32_t mixBlockBeginIndex;
            totalVarNum = 0;
            mixBlockBeginIndex = 0;
            for(uint index = 0; index < m_cycReadVector.size(); ++index)
            {
                totalVarNum += m_cycReadVector.at(index).len;
                if(totalVarNum > static_cast<uint32_t>( m_mixLenMax ) || (index - mixBlockBeginIndex) >= static_cast<uint32_t>( m_mixBlockNumMax ) )
                {//达到分块条件
                    m_cycMixedRead.push_back(index);
                    totalVarNum = static_cast<uint32_t>( m_cycReadVector.at(index).len );
                    mixBlockBeginIndex = index;
                }
            }
        }
    }
}

int32_t Device::AnalysisFrame(Frame &recvF)
{
    static int32_t ret;
    ret = 0;

    //1.校验帧格式
    if(m_MDevice)
        ret = m_MDevice->CheckFrame(recvF);
    if(0 != ret)
    {//帧校验失败退出
        //通讯完成处理
        //DealCommunicationFinish(m_optFlag,recvF,ret);
        //qDebug()<<__func__<<"CheckFrame:"<<ret;
        return ret;
    }

    //2.解析帧数据
    switch (m_optFlag) {
    case cycleRead:
    case conditionRead:
    {
       for(uint32_t i = 0;
           i < recvF.readList.size() &&
           i < recvF.recvDataBeginIndex.size() &&
           i < recvF.recvDataLen.size();
           i++)
       {
           //qDebug()<<__func__<<i<<QString::fromStdString(recvF.readList.at(i).reg)<<recvF.readList.at(i).index/16<<recvF.readList.at(i).len/16;
           //解析前进行数据特殊处理
           if(m_MDevice)
           {
               if(-1 == m_MDevice->DealRecvData(recvF.readList.at(i).reg,
                                               recvF.readList.at(i).index,
                                               recvF.GetRecvBuff(),
                                               recvF.recvDataBeginIndex[i],
                                               recvF.recvDataLen[i]))
               {//子类已经解析完了,下一个地址块
                   continue;
               }
           }
           //开始解析
           ret = SetDataFromAddr(recvF.readList.at(i).reg,
                                 recvF.readList.at(i).index,
                                 recvF.recvDataLen.at(i),
                                 recvF.GetRecvBuff()+recvF.recvDataBeginIndex.at(i));
       }
    }
        break;
    case write:
    {
        //无需解析数据
    }
        break;
    }

    //通讯完成处理
    //DealCommunicationFinish(m_optFlag,recvF,0);

    return 0;
}

void Device::DealCommunicationFinish(int32_t optFlag,const Protocol::DataWCallback &RWCallback, int32_t result)
{
    switch (optFlag) {
    case cycleRead:
    {
        if(continuousRead ==  m_protocolReadMode)
        {//全连续读
            m_curReadIndex++;
            if(m_curReadIndex >= m_cycReadVector.size())
            {
                m_curReadIndex = 0;
            }
        }
        else
        {//全混合读
            m_curReadIndex++;
            if(m_curReadIndex >= m_cycMixedRead.size())
            {
                m_curReadIndex = 0;
            }
        }
    }
        break;
    case conditionRead:
    {
        m_conditionReadLock.lock();
        if(0 < m_conditonReadList.size())
        {
            if(RWCallback)
            {//回调
                RWCallback(m_conditonReadList.front(),result);
            }
            m_conditonReadList.pop_front();
        }
        m_conditionReadLock.unlock();
    }
        break;
    case write:
    {
        m_writeLock.lock();
        if(0 < m_writeList.size() )
        {
            if(RWCallback)
            {//回调
                RWCallback(m_writeList.front(),result);
            }
            m_writeList.pop_front();
        }
        m_writeLock.unlock();
    }
        break;
    }
}

int32_t Device::BuildSendFrame(std::vector<Protocol::AddrInfo> &addrVec)
{
    //static std::vector<AddrInfo> addrVec;
    static int32_t ret;
    ret = 0;
    static std::vector<Protocol::AddrInfoForRW> addrInfo;

    switch (m_optFlag) {
    case cycleRead:
    {
        if(m_cycReadLock.try_lock())
        {//如果获取到锁了就往下，循环读设置阻塞了就退出下一次，优先级让给条件读和写入操作
            if(0 == m_cycReadVector.size())
            {//检查是否有循环读项
                m_cycReadLock.unlock();
                return 0;
            }
            addrVec.clear();
            //后续加缓存模式
            if(continuousRead ==  m_protocolReadMode)
            {//全连续读
                if(m_curReadIndex < m_cycReadVector.size())
                {
                    addrVec.push_back(m_cycReadVector.at(m_curReadIndex));
                }
            }
            else
            {//全混合读
                if(m_curReadIndex < m_cycMixedRead.size())
                {
                    if(m_curReadIndex == m_cycMixedRead.size()-1)
                    {//最后一组
                        for (uint32_t i = m_cycMixedRead.at(m_curReadIndex) ; i < m_cycReadVector.size(); ++i)
                        {
                            addrVec.push_back(m_cycReadVector.at(i));
                        }
                    }
                    else
                    {
                        for (uint32_t i = m_cycMixedRead.at(m_curReadIndex) ; i < m_cycReadVector.size() && i < m_cycMixedRead.at(m_curReadIndex+1); ++i)
                        {
                            addrVec.push_back(m_cycReadVector.at(i));
                        }
                    }
                }
            }

            //处理是否开启了循环读取帧缓存模式
            if(m_bCycReadFrameCache)
            {
                std::map<uint32_t,std::deque<Frame> >::iterator itF = m_cycFrameMapOfCurReadIndex.find(m_curReadIndex);
                if(itF != m_cycFrameMapOfCurReadIndex.end())
                {//有
                    m_curFrameList.assign(itF->second.begin(),itF->second.end());
                }
                else
                {//没有
                    if(m_MDevice)
                    {
                        ret = m_MDevice->ReadFrame(addrVec);
                    }
                    if(0 < m_curFrameList.size() )
                    {
                        m_cycFrameMapOfCurReadIndex[m_curReadIndex] = m_curFrameList;
                    }
                }
            }
            else
            {
                if(m_MDevice)
                {
                    ret = m_MDevice->ReadFrame(addrVec);
                }
            }

            m_cycReadLock.unlock();
        }
    }
        break;
    case conditionRead:
    {
        addrVec.clear();
        m_conditionReadLock.lock();
        if(0 == m_conditonReadList.size())
        {//检查是否为空
            m_conditionReadLock.unlock();
            return 0;
        }
        if(0 == m_syncRWFlag)
        {//异步从头
            addrVec.push_back(AddrInfoForRW2AddrInfo(m_conditonReadList.front()));
            if(m_MDevice)
                ret = m_MDevice->ReadFrame(addrVec,m_conditonReadList.front().callback);
        }
        else
        {//同步从后面拿
            addrVec.push_back(AddrInfoForRW2AddrInfo(m_conditonReadList.back()));
            if(m_MDevice)
                ret = m_MDevice->ReadFrame(addrVec,m_conditonReadList.back().callback);
        }
        m_conditionReadLock.unlock();
    }
        break;
    case write:
    {
        addrInfo.clear();
        m_writeLock.lock();
        if(0 == m_writeList.size())
        {//检查是否为空
            m_writeLock.unlock();
            return 0;
        }
        if(0 == m_syncRWFlag)
        {//异步从头
            addrInfo.push_back(m_writeList.front());
            addrVec.clear();
            addrVec.push_back(AddrInfoForRW2AddrInfo(m_writeList.front()));
        }
        else
        {//同步从后面拿
            addrInfo.push_back(m_writeList.back());
            addrVec.clear();
            addrVec.push_back(AddrInfoForRW2AddrInfo(m_writeList.back()));
        }
        m_writeLock.unlock();
        if(m_MDevice)
            ret = m_MDevice->WriteFrame(addrInfo);
    }
        break;
    }
    return ret;
}

void Device::DealIndexAlignOfByteForRead(uint64_t &index, uint64_t &len)
{//以8为单位对齐
   len += index%8;
   index -= index%8;
}

AddrInfo Device::AddrInfoForRW2AddrInfo(const AddrInfoForRW &addrForRW)
{
    static AddrInfo aInfo;
    aInfo.reg = addrForRW.reg;
    aInfo.index = addrForRW.index;
    switch (addrForRW.dataType) {
    case Protocol::bit:
    {
        aInfo.len = 1*addrForRW.num;
        DealIndexAlignOfByteForRead(aInfo.index,aInfo.len);
    }
        break;
    case Protocol::bit8:
    case Protocol::ubit8:
    {
        aInfo.index = aInfo.index - aInfo.index%8;
        aInfo.len = 8*addrForRW.num;
    }
        break;
    case Protocol::ubit16:
    case Protocol::bit16:
    {
        aInfo.index = aInfo.index - aInfo.index%8;
        aInfo.len = 16*addrForRW.num;
    }
        break;
    case Protocol::ubit32:
    case Protocol::bit32:
    case Protocol::float32:
    {
        aInfo.index = aInfo.index - aInfo.index%8;
        aInfo.len = 32*addrForRW.num;
    }
        break;
    case Protocol::ubit64:
    case Protocol::bit64:
    case Protocol::double64:
    {
        aInfo.index = aInfo.index - aInfo.index%8;
        aInfo.len = 64*addrForRW.num;
    }
        break;
    case Protocol::string:
    {
        aInfo.index = aInfo.index - aInfo.index%8;
        //字符串个数和长度置换
        uint32_t len;
        uint64_t num = addrForRW.num;
        GetTypeStringOfNumAndLen(num,num,len);
        aInfo.len = num*len*8;
    }
        break;
    }
    return aInfo;
}

int32_t Device::GetLastData(AddrInfoForRW &addr)
{
    int ret = 0;
    uint32_t offset = 1;
    //计算偏移
    GetBitNumFromDatatype(addr.dataType,offset);
    //读值
    if(Protocol::string != addr.dataType)
    {
        addr.valueVec.resize(addr.num);
        //开始
        uint64_t index = addr.index;
        uint32_t len = static_cast<uint32_t>(addr.num*offset);
        CompleteByByte(index,len);
        std::vector<char> bufv;
        bufv.resize(len/8);
        //一次性获取数据
        GetDataFromAddr(addr.reg,index,len,bufv.data());
        if(1 != offset)
        {//字
            //开始处理一个个数据
            uint32_t byteOffset = 0;
            for (uint i = 0; i < addr.num; ++i)
            {
                //处理字节序
                byteOffset = (offset*i)>>3;
                if(m_MDevice)
                    m_MDevice->DealByteOrder(bufv.data()+byteOffset,addr.dataType);
                //数据转换
                Char2Variant(addr.dataType,bufv.data()+byteOffset,addr.valueVec[i]);
            }
        }
        else
        {//位
            //开始处理一个个数据
            uint64_t bitOffset = 0;
            for (uint i = 0; i < addr.num; ++i)
            {
                //计算偏移
                bitOffset = (addr.index -index + i);
                //数据转换
                addr.valueVec[i] = GetBitOfByte(bufv.data()+bitOffset/8,bitOffset%8);
            }
        }
    }
    else
    {//string
      std::string sData;

      //标签-数据类型字符串处理长度
      if( "TAG" == addr.sIndex)
      {
         Protocol::DataVariant dataV;
         if(-1 != GetDataFromAddrAndDatatype(addr.reg,0,Protocol::ubit16,dataV))
         {
           addr.num = ( std::get<unsigned short>(dataV) )*8 ;
         }
         else
         {
           addr.num = 8;
         }
      }

      //字符串个数和长度置换

      uint32_t len;
      uint64_t num = addr.num;
      GetTypeStringOfNumAndLen(num,num,len);

      //获取
      addr.valueVec.resize(num);
      for (uint32_t i = 0; i < num; ++i)
      {
          sData.resize(len);
          if(-1 == GetDataFromAddr(addr.reg,addr.index+i*len*8,len*8,sData.data()) )
          {
              ret = Protocol_Rtn_Fail;
          }
          addr.valueVec.at(i) = sData;
      }
    }
    return ret;
}

int32_t Device::SetLastData(const Protocol::AddrInfoForRW &addr)
{
    int32_t ret = Protocol_Rtn_Success;
    uint32_t offset = 1;

    //获取寄存器信息
    Protocol::RegInfo rInfo;
    if(Protocol_Rtn_Fail == GetRefInfoFromReg(addr.reg,rInfo))
    {
        return Protocol_Rtn_Fail;
    }

    if(Protocol::WriteOnly != rInfo.rwFlag)
    {//非只写类型才有地址缓存
        //计算偏移
        GetBitNumFromDatatype(addr.dataType,offset);
        //值更新到地址缓存
        if(Protocol::string != addr.dataType)
        {
            //开始
            uint64_t index = addr.index;
            uint32_t len = static_cast<uint32_t>(addr.num*offset);
            CompleteByByte(index,len);
            std::vector<char> bufv;
            bufv.resize(len/8);
            if(1 != offset)
            {//字
                //写入数据处理
                uint32_t byteOffset = 0;
                for (uint32_t num = 0; num < addr.num && num < addr.valueVec.size(); ++num)
                {
                    //计算字节偏移
                    byteOffset = (offset*num)>>3;
                    //数据转换
                    Variant2Char(addr.valueVec.at(num),bufv.data()+byteOffset);
                    //字节序
                    if(m_MDevice)
                        m_MDevice->DealByteOrder(bufv.data()+byteOffset,addr.dataType);
                }
            }
            else
            {//位
                uint64_t bitOffset = 0;
                for (uint32_t num = 0; num < addr.num && num < addr.valueVec.size(); ++num)
                {
                    //计算偏移
                    bitOffset = (addr.index -index + num);
                    //数据转换
                    if(std::get<char>(addr.valueVec[num]))
                    {
                        SetBitOfByte(bufv.data()+bitOffset/8,bitOffset%8,true);
                    }
                    else
                    {
                        SetBitOfByte(bufv.data()+bitOffset/8,bitOffset%8,false);
                    }
                }
            }
            //数据写入
            SetDataFromAddr(addr.reg,index,len,bufv.data());
        }
        else
        {//string
            std::string sData;

            //字符串个数和长度置换
            uint32_t len;
            uint64_t num = addr.num;
            GetTypeStringOfNumAndLen(num,num,len);

            //写入
            for (uint32_t i = 0; i < num; ++i)
            {
                sData = std::get<std::string>(addr.valueVec.at(i));
                sData.resize(len);
                if(-1 == SetDataFromAddr(addr.reg,addr.index+i*len*8,len*8,sData.data()))
                {
                    ret = Protocol_Rtn_Fail;
                }
            }
        }
    }
    return ret;
}

int32_t Device::GetDataFromAddrAndDatatype(const std::string &reg, const uint64_t &index, const Protocol_DataType &dataType, DataVariant &data)
{
    Protocol::AddrInfoForRW addr;
    addr.reg = reg;
    addr.index = index;
    addr.num = 1;
    addr.dataType = dataType;
    GetLastData(addr);
    if(0 < addr.valueVec.size() )
    {
        data = addr.valueVec.front();
        return 0;
    }
    return -1;
}

int32_t Device::SetDataFromAddrAndDatatype(const std::string &reg, const uint64_t &index, const Protocol_DataType &dataType, const DataVariant &data)
{
    Protocol::AddrInfoForRW addr;
    addr.reg = reg;
    addr.index = index;
    addr.num = 1;
    addr.dataType = dataType;
    addr.valueVec.push_back(data);
    return SetLastData(addr);
}

int32_t Device::GetDataFromAddr(const std::string &reg, uint64_t index, uint32_t len, char *data)
{
    //按字节对齐
    CompleteByByte(index,len);

    Data_Area dataArea;
    dataArea.reg = reg;
    dataArea.index = index;
    uint64_t offset = 0;
    char * dst = nullptr;

    //开始解析
    uint32_t freeBitLen =  ((ADDRESS_AREA_UNIT)*8) - (dataArea.index%(ADDRESS_AREA_UNIT*8));
    if( freeBitLen >= len )
    {//在一个地址块中
        std::set<Data_Area>::iterator it =  GetDataAreaIterator(dataArea);
        if(it == m_dataArea.end())
        {
            return -1;
        }
        offset = dataArea.index - it->index;
        //地址内存复制
        memcpy(data,(*it).data+offset/8,len/8);
    }
    else
    {//分散在多个地址块中
        uint32_t leftByteLen;
        uint32_t totalByte;
        uint32_t rightByteLen;
        uint32_t midUnitLen;
        leftByteLen =  (freeBitLen/8);
        totalByte = len/8;
        rightByteLen = (totalByte - leftByteLen)%ADDRESS_AREA_UNIT;
        midUnitLen = (totalByte - leftByteLen)/ADDRESS_AREA_UNIT;

        //1.left
        std::set<Data_Area>::iterator it =  GetDataAreaIterator(dataArea);
        if(it == m_dataArea.end())
        {
            return -1;
        }
        offset = dataArea.index - it->index;
        //内存复制
        memcpy(data,(*it).data+offset/8,leftByteLen);

        dataArea.index += leftByteLen*8;

        //2.mid
        for (uint32_t times = 0 ; times < midUnitLen ; ++times)
        {
            std::set<Data_Area>::iterator it2 =  GetDataAreaIterator(dataArea);
            if(it2 == m_dataArea.end())
            {
                return -1;
            }
            //内存复制
            dst = data +times*ADDRESS_AREA_UNIT+leftByteLen;
            memcpy(dst,(*it2).data,ADDRESS_AREA_UNIT);
            dataArea.index += ADDRESS_AREA_UNIT*8;
        }

        //3.right
        std::set<Data_Area>::iterator it2 =  GetDataAreaIterator(dataArea);
        if(it2 == m_dataArea.end())
        {
            return -1;
        }
        //内存复制
        dst = data+midUnitLen*ADDRESS_AREA_UNIT+leftByteLen;
        memcpy(dst,(*it2).data,rightByteLen);
    }
    return 0;
}

int32_t Device::SetDataFromAddr(const std::string &reg, uint64_t index, uint32_t len, char *data)
{
    //按字节补齐长度
    CompleteByByte(index,len);

    //初始化数据
    Data_Area dataArea;
    dataArea.reg = reg;
    dataArea.index = index;
    uint64_t offset = 0;
    char * dataA = nullptr;
    char * dst = nullptr;
    char * src = nullptr;
    uint32_t memCpLen = 0;

    Protocol::AddrInfo addrInfo;
    addrInfo.reg = reg;
    addrInfo.index = index;
    addrInfo.len = len;

    //开始解析
    uint32_t freeBitLen =  ((ADDRESS_AREA_UNIT)*8) - (dataArea.index%(ADDRESS_AREA_UNIT*8));
    if( freeBitLen >= len )
    {//在一个地址块中
        std::set<Data_Area>::iterator it =  GetDataAreaIterator(dataArea);
        if(it == m_dataArea.end())
        {
            return -1;
        }
        dataA =  const_cast<char *>((*it).data);
        offset = dataArea.index - it->index;
        //开始处理地址变化回调
        dst = dataA+offset/8;
        src = data;
        memCpLen = len/8;
        if(0 != Strcmp(dst,src,memCpLen))
        {
            //地址内存复制
            memcpy(dst,src,memCpLen);
            //回调
            DealAddrValueChangeCB(addrInfo,0);
        }
    }
    else
    {//分散在多个地址块中
        bool bCallbackFlag = false;
        uint32_t leftByteLen =  (freeBitLen/8);
        uint32_t totalByte = len/8;
        uint32_t rightByteLen = (totalByte - leftByteLen)%ADDRESS_AREA_UNIT;
        uint32_t midUnitLen = (totalByte - leftByteLen)/ADDRESS_AREA_UNIT;

        //1.left
        std::set<Data_Area>::iterator it =  GetDataAreaIterator(dataArea);
        if(it == m_dataArea.end())
        {
            return -1;
        }
        dataA =  const_cast<char *>((*it).data);
        offset = dataArea.index - it->index;
        //变化回调
        dst = dataA+offset/8;
        src = data;
        memCpLen = leftByteLen;
        if(0 != Strcmp(dst,src,memCpLen))
        {
            bCallbackFlag = true;
            memcpy(dst,src,memCpLen);
        }
        dataArea.index += leftByteLen*8;

        //2.mid
        for (uint32_t times = 0 ; times < midUnitLen ; ++times)
        {
            std::set<Data_Area>::iterator it2 =  GetDataAreaIterator(dataArea);
            if(it2 == m_dataArea.end())
            {
                return -1;
            }
            dataA =  const_cast<char *>((*it2).data);
            //变化回调
            dst = dataA;
            src = data +times*ADDRESS_AREA_UNIT+leftByteLen;
            memCpLen = ADDRESS_AREA_UNIT;
            if(0 != Strcmp(dst,src,memCpLen))
            {
                bCallbackFlag = true;
                //内存复制
                memcpy(dst,src,memCpLen);
            }
            dataArea.index += ADDRESS_AREA_UNIT*8;
        }

        //3.right
        std::set<Data_Area>::iterator it2 =  GetDataAreaIterator(dataArea);
        if(it2 == m_dataArea.end())
        {
            return -1;
        }
        dataA =  const_cast<char *>((*it2).data);

        //变化回调
        dst = dataA;
        src = data+midUnitLen*ADDRESS_AREA_UNIT+leftByteLen;
        memCpLen = rightByteLen;
        if(0 != Strcmp(dst,src,memCpLen))
        {
            bCallbackFlag = true;
            //内存复制
            memcpy(dst,src,memCpLen);
        }

        //4.判断是否变化回调
        if(bCallbackFlag)
        {
            DealAddrValueChangeCB(addrInfo,0);
        }
    }
    return 0;
}

int32_t Device::GetVersion(std::string &sVersion)
{
    if(m_MDevice)
    {
        return m_MDevice->GetVersion(sVersion);
    }
    return -1;
}

int32_t Device::Process(std::vector<AddrInfo> &addrList)
{//同步
    static int ret;
    ret = 0;
    //组帧
    if(0 == m_syncRWFlag)
    {//异步按优先级来
        m_writeLock.lock();
        static uint64_t writeNum;
        writeNum = m_writeList.size();
        m_writeLock.unlock();

        m_conditionReadLock.lock();
        static uint64_t condReadeNum;
        condReadeNum = m_conditonReadList.size();
        m_conditionReadLock.unlock();

        if(0 < writeNum && m_writeTimes <= m_deviceConfig.protocolInfo.RWThreshold)
        {//连续写入
            m_writeTimes++;
            m_optFlag = Device::write;
            BuildSendFrame(addrList);
        }
        else if (0 < condReadeNum && m_conditonReadTimes <= m_deviceConfig.protocolInfo.RWThreshold)
        {//条件读
            m_writeTimes = 0;
            m_conditonReadTimes++;
            m_optFlag = Device::conditionRead;
            BuildSendFrame(addrList);
        }
        else
        {//循环读
            m_conditonReadTimes = 0;
            m_optFlag = Device::cycleRead;
            BuildSendFrame(addrList);
        }
    }
    else
    {//同步读写
        switch (m_syncRWFlag) {
        case Device::conditionRead:
        {
            m_optFlag = Device::conditionRead;
            BuildSendFrame(addrList);
        }
            break;
        case Device::write:
        {
            m_optFlag = Device::write;
            BuildSendFrame(addrList);
        }
            break;
        }
    }

    //开始收发数据
    if( m_comm && 0 < m_curFrameList.size() )
    {
        //先取出回调
        Protocol::DataWCallback RWCallback = m_curFrameList.back().RWCallback;
        //开始处理数据帧
        while (0 != m_curFrameList.size())
        {
            //一次通讯前处理
            if(m_MDevice)
            {
                m_MDevice->BeforeCommunication();
            }
            //发送
            ret = m_comm->SendData(m_curFrameList.front().GetSendBuff(), m_curFrameList.front().sendLen);
            if(ret < 0)
            {//重连
                m_comm.get()->CloseComm();
                m_comm.get()->OpenComm();
            }
            //开始接收处理数据
            ret = RecvData();
            if(Protocol_Process_R_Suc != ret && Protocol_Process_W_Suc != ret)
            {//通信（重试过）不成功,直接清掉当前发送队列
                m_curFrameList.clear();
                break;
            }
        }
        //通讯完成处理
        if(Protocol_Process_R_Suc == ret || Protocol_Process_W_Suc == ret)
        {//通讯成功
            DealCommunicationFinish(m_optFlag,RWCallback,0);
        }
        else
        {//通讯失败
            DealCommunicationFinish(m_optFlag,RWCallback,-1);
        }
    }
    return ret;
}

//从站模式下数据回调
//返回值：接收的数据处理了多少，单位字节
int Device::Slave_ReadReady_CB(int code, char *buf, uint32_t len,char **sBuf, uint32_t *sLen)
{
    if(0 == code && 0 < len )
    {//有数据
        if(m_MDevice)
        {
            m_MDevice->DealDataOfSlave(buf,len,sBuf,sLen);
        }
    }
    //处理完成
    return static_cast<int32_t>(len);
}

//标签处理转换
int32_t Device::TagDeal(AddrInfoForRW &addr)
{
#ifdef TAG_MODE
    //协议标签模式
    //补全一些信息
    RegInfo regInfo;
    if(Protocol_Rtn_Success == GetRefInfoFromReg(addr.reg,regInfo))
    {
        if(string == regInfo.dataType)
        {//字符串数据类型的标签前两个字节来存储字节长度
          addr.index = 2*8;
        }
        else
        {
          addr.index = 0;
        }
        addr.dataType = regInfo.dataType;
        addr.sIndex = "TAG";//来标识该地址是标签地址
        addr.num = 1;
        return Protocol_Rtn_Success;
    }
    return Protocol_Rtn_Fail;
#else
//标签别名
    std::map<std::string,Protocol::Tag_Info>::iterator it =  m_tagInfoMap.find(addr.reg);
    if(it != m_tagInfoMap.end())
    {//标签别名
       addr.reg = it->second.addrInfo.reg;
       addr.num = 1;
       addr.index = it->second.addrInfo.index;
       addr.dataType = it->second.dataType;
       return Protocol_Rtn_Success;
    }
    return Protocol_Rtn_Fail;
#endif
}

int32_t Device::TagDeal(AddrInfo &addr)
{
#ifndef TAG_MODE
    std::map<std::string,Protocol::Tag_Info>::iterator it =  m_tagInfoMap.find(addr.reg);
    if(it != m_tagInfoMap.end())
    {//标签别名
       addr.reg = it->second.addrInfo.reg;
       addr.index = it->second.addrInfo.index;
       uint32_t tmpLen = 0;
       GetBitNumFromDatatype(it->second.dataType,tmpLen);
       addr.len = tmpLen;
       return Protocol_Rtn_Success;
    }
    return Protocol_Rtn_Fail;
#else
    Q_UNUSED(addr);
#endif
    return Protocol_Rtn_Success;
}

void Device::GetBitNumFromDatatype(Protocol_DataType dataType, uint32_t &bitNum)
{
    switch (dataType) {
    case bit:
    {
        bitNum = 1;
    }
        break;
    case bit8:
    case ubit8:
    {
        bitNum = 8;
    }
        break;
    case bit16:
    case ubit16:
    {
        bitNum = 16;
    }
        break;
    case bit32:
    case ubit32:
    case float32:
    {
        bitNum = 32;
    }
        break;
    case bit64:
    case ubit64:
    case double64:
    {
        bitNum = 64;
    }
        break;
    case string:
    {
        bitNum = 0;
    }
    }
}

void Device::GetTypeStringOfNumAndLen(uint64_t in_num, uint64_t &out_num, uint32_t &out_len)
{
    //转换
    uint64_t num = in_num >> 3;

    //每个字符串的长度
    out_len = num & 0xffffffff;

    //字符串个数
    num = (num >> 32) & 0xffffffff;
    num = (num == 0)?1:num;

    out_num = static_cast<uint32_t>( num );
}

void Device::InitFrame(Frame &frame)
{
    frame.send.clear();
    frame.recv.clear();
    frame.readList.clear();
    frame.recvDataBeginIndex.clear();
    frame.recvDataLen.clear();
    frame.RWCallback = nullptr;
}

int32_t Device::Process_Slave()
{//同步
    static int ret;
    ret = 0;
    if( m_comm )
    {
        //主动收发模式和回调模式
        if(Comm_Type_Tcp_Server != m_deviceConfig.commInfo.commType)
        {
            static char recvBuff[RECV_BUFF_MAX] = {0};
            static int32_t recvLen;
            static int32_t ilen;
            ilen = RECV_BUFF_MAX;
            recvLen = m_comm->RecvData(recvBuff,ilen,0);
            if(0 > recvLen)
            {
                m_comm.get()->CloseComm();
                m_comm.get()->OpenComm();
            }
            else
            {
                if(m_MDevice)
                {
                    char * sendBuf = nullptr;
                    uint32_t sendLen = 0;
                    m_MDevice->DealDataOfSlave(recvBuff,static_cast<uint32_t>(recvLen),&sendBuf,&sendLen);
                    if( 0 < sendLen )
                    {
                        m_comm->SendData(sendBuf,static_cast<uint32_t>( sendLen ));
                    }
                }
            }
        }
        else
        {
           //设置回调
           m_comm->OpenComm(0,500,nullptr,nullptr,
                            std::bind(&Device::Slave_ReadReady_CB,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,
                                                            std::placeholders::_4,std::placeholders::_5));
           //启动事件循环
           m_comm->AsynProcess();
        }
    }
    return ret;
}

int32_t Device::Process_All()
{
    static int32_t ret = 0;
    if(m_threadIsRun && m_deviceConfig.commInfo.isOnline)
    {
        static std::vector<AddrInfo> addrList;
        addrList.clear();
        if(Master == m_deviceConfig.protocolInfo.masterOrSlave)
        {//主站模式
            m_lock.lock();
            ret = Process(addrList);
            m_lock.unlock();
        }
        else if(Slave == m_deviceConfig.protocolInfo.masterOrSlave)
        {//从站模式
           m_lock.lock();
           ret = Process_Slave();
           m_lock.unlock();
        }
        else
        {//混合模式
          m_lock.lock();
          //先检查有无推送的请求
          Process_Slave();
          //再主动请求
          ret = Process(addrList);
          m_lock.unlock();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(m_deviceConfig.protocolInfo.communicationInterval));
    }
    else
    {//暂停
        std::this_thread::sleep_for(std::chrono::milliseconds(m_deviceConfig.protocolInfo.timeout));
    }
    return ret;
}

void Device::Run()
{//线程run
    m_threadIsOver = false;
    while (m_bThreadFlag)
    {
        //处理主从通讯
        Process_All();
    }
    m_threadIsOver = true;
}

void Device::StopThread()
{
    if(m_processThread)
    {//关闭线程
        m_bThreadFlag = false;
        m_processThread->join();
    }
}
