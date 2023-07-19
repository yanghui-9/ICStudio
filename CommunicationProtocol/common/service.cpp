#include <QString>
#include <QStringList>

#include "service.h"
#include "typedefheader.h"
#include "protocol_interface.h"

#include "../../communication/communicationdef.h"
#include "../../communication/communicationglobal.h"

#include "glog/logging.h"
#include "../../Interface/IApp.h"

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
    //开始接收数据.
    static char recvBuff[RECV_BUFF_MAX] = {0};
    int ret = m_comm->RecvData(recvBuff,m_curFrameList.front().recvLen,
                             m_deviceConfig.protocolInfo.timeout);
    #ifdef DEBUG_LOG
    LOG(INFO)<<"******"<<"Recv:"<<m_curFrameList.size()<<" "<<m_curFrameList.front().recvLen<<" "<<ret;
    #endif
    m_curFrameList.front().actualRecvLen = ret;
    if(ret < 0)
    {//重连.
        m_comm.get()->CloseComm();
        ushort ret =  static_cast<ushort>( m_comm.get()->OpenComm() );
        //if(0 != ret)
        {//连接失败.
            SetDataFromAddrAndDatatype(Protocol_Status_Reg_Name,Protocol_Status_CommStatus,Protocol::ubit16,ret);
        }
    }
    else if (ret > 0)
    {
       memcpy(m_curFrameList.front().GetRecvBuff(ret),recvBuff,static_cast<uint32_t>(ret));
    }
    ret = AnalysisFrame(m_curFrameList.front());
    #ifdef DEBUG_LOG
    LOG(INFO)<<"******"<<"Analysis-Frame-Reslut:"<<ret;
    #endif
    switch (ret) {
    case error:
    {//校验或者解析失败.
        if(++m_errTimes >= m_deviceConfig.protocolInfo.retryTimes)
        {//达到重试次数.
            m_errTimes = 0;
            switch (m_optFlag) {
            case cycleRead:
            case conditionRead:
            {
                if(m_curFrameList.front().actualRecvLen > 0)
                {//有数据接收到.
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
                {//有数据接收到.
                    ret = Protocol_Process_W_CheckError;
                }
                else
                {
                    ret = Protocol_Process_W_NoData;
                }
            }
                break;
            }
            //m_curFrameList.clear();
        }
        else
        {
          ret = Protocol_Process_Retry;
        }
    }
        break;
    case retry:
    {//再读.
       ret = RecvData();
    }
        break;
    default:
    {
        //通讯正常完成，出.
        m_errTimes = 0;//重置重试次数
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

    //返回结果.
    return ret;
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
    else
    {
#ifdef TAG_MODE
        info.unitBitNum = 1;
        info.rwFlag = Protocol::readWrite;
        info.dataType = Protocol::null_dType;
        return Protocol_Rtn_Success;
#endif
    }
    return Protocol_Rtn_Fail;
}

int32_t Device::GetRegUint(const std::string &reg)
{
    RegInfo info;
    if(Protocol_Rtn_Fail == GetRefInfoFromReg(reg,info))
    {
        return  Protocol_Rtn_Fail;
    }
    return static_cast<int32_t>( info.unitBitNum );
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
        {//位.
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
    //初始化通讯接口.
    if(comm)
    {
        m_comm = comm;
        if(m_comm)
        {
           int32_t ret = m_comm->OpenComm();
           if(0 != ret)
           {//通讯打开失败.
              SetDataFromAddrAndDatatype(Protocol_Status_Reg_Name,Protocol_Status_CommStatus,ubit16,static_cast<ushort>( ret ));
           }
        }
    }
    else
    {
        if (!m_comm)
        {
            std::shared_ptr<ICommunication> ptrComm(CreateComm(static_cast<ICommunication::Communication_Type>( m_deviceConfig.commInfo.commType ),
                                                               m_deviceConfig.commInfo.sCommPar));
            if(ptrComm)
            {
                int32_t ret = ptrComm->OpenComm();
                if(0 != ret)
                {//通讯打开失败.
                   SetDataFromAddrAndDatatype(Protocol_Status_Reg_Name,Protocol_Status_CommStatus,ubit16,static_cast<ushort>( ret ));
                }
                m_comm = ptrComm;
            }
        }
        comm = m_comm;
    }

    //Tcp服务端回调初始化.
    m_comm->OpenComm(0,500,nullptr,nullptr,
                     std::bind(&Device::Slave_ReadReady_CB,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,
                                                     std::placeholders::_4,std::placeholders::_5));
}

void Device::UpdateConfig(const Device_Config &config)
{
    //判断线程模式.
    if(config.commInfo.isThread)
    {//线程process
        if(m_processThread)
        {//线程已经创建.
            m_threadIsRun = true;
        }
        else
        {
            m_threadIsRun = true;
            m_processThread = std::make_shared<std::thread>(&Device::Run,this);
        }
    }
    else
    {//非线程模式.
        if(m_processThread)
        {//线程已经创建，进入暂停模式.
            m_threadIsRun = false;
        }
    }

    //判断通讯日志开启.
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
    case bit:
    {//char
        if(std::get<bool>(dataV))
        {
            value[0] = 1;
        }
        else
        {
            value[0] = 0;
        }
    }
        break;
    case bit8:
    {//char
        value[0] = std::get<char>(dataV);
    }
        break;
    case ubit8:
    {//uchar
        value[0] = static_cast<char>( std::get<unsigned char>(dataV) );
    }
        break;
    case bit16:
    {//short
        short s;
        s = std::get<short>(dataV) ;
        memcpy(value,&s,2);
    }
        break;
    case ubit16:
    {//ushort
        unsigned short us;
        us = std::get<unsigned short>(dataV) ;
        memcpy(value,&us,2);
    }
        break;
    case ubit32:
    {//uint
        uint32_t ui;
        ui = std::get<uint32_t>(dataV) ;
        memcpy(value,&ui,4);
    }
        break;
    case bit32:
    {//int
        int32_t i;
        i = std::get<int32_t>(dataV) ;
        memcpy(value,&i,4);
    }
        break;
    case float32:
    {//float
        float i;
        i = std::get<float>(dataV) ;
        memcpy(value,&i,4);
    }
        break;
    case ubit64:
    {//uint64
        uint64_t i;
        i = std::get<uint64_t>(dataV) ;
        memcpy(value,&i,8);
    }
        break;
    case bit64:
    {//int64
        int64_t i;
        i = std::get<int64_t>(dataV) ;
        memcpy(value,&i,8);
    }
        break;
    case double64:
    {//double
        double i;
        i = std::get<double>(dataV) ;
        memcpy(value,&i,8);
    }
        break;
    case string:
    {//string
        std::string s;
        s = std::get<std::string>(dataV) ;
        memcpy(value,s.data(),s.size());
    }
        break;
    }
}

void Device::Char2Variant(Protocol_DataType dataType,std::vector<char> &valueV, DataVariant &data)//std::vector<char>
{
    char *value = valueV.data();
    //开始数据转换.
    switch (dataType) {
    case Protocol::bit:
    {
        if(0 == value[0])
        {
            data = false;
        }
        else
        {
            data = true;
        }
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
    case Protocol::string:
    {
        data = std::string(value,valueV.size());
    }
        break;
    default:
    {
        //
    }
    }
}

void Device::StringToCharOfDataType(Protocol_DataType dataType, const std::string &sData, char *data)
{
    //开始数据转换.
    switch (dataType) {
    case Protocol::bit:
    case Protocol::bit8:
    case Protocol::ubit8:
    {
        int iValue = std::stoi(sData);
        memcpy(data,&iValue,1);
    }
        break;
    case Protocol::ubit16:
    {
        ushort iValue = static_cast<ushort>( std::stoi(sData) );
        memcpy(data,&iValue,2);
    }
        break;
    case Protocol::bit16:
    {
        short iValue = static_cast<short>( std::stoi(sData) );
        memcpy(data,&iValue,2);
    }
        break;
    case Protocol::ubit32:
    {
        uint32_t iValue = static_cast<uint32_t>( std::stoi(sData) );
        memcpy(data,&iValue,4);
    }
        break;
    case Protocol::bit32:
    {
        int32_t iValue = static_cast<int32_t>( std::stoi(sData) );
        memcpy(data,&iValue,4);
    }
        break;
    case Protocol::float32:
    {
        float iValue = std::stof(sData);
        memcpy(data,&iValue,4);
    }
        break;
    case Protocol::ubit64:
    {
        uint64_t iValue = static_cast<uint64_t>( std::stoull(sData) );
        memcpy(data,&iValue,8);
    }
        break;
    case Protocol::bit64:
    {
        int64_t iValue = static_cast<int64_t>( std::stoull(sData) );
        memcpy(data,&iValue,8);
    }
        break;
    case Protocol::double64:
    {
        double iValue = std::stod(sData);
        memcpy(data,&iValue,8);
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

int32_t Device::SetRegValueChangeCallback(std::vector<AddrInfoForRW> &addrVec,
                                  Protocol::Add_Type addType )
{
    return m_DataAreaDeal.SetRegValueChangeCallback(addrVec,addType);
}


void Device::MergeAddr(Type type)
{
    //Not implemented at present
    if( cycleRead == type)
    {//循环读地址分块.
        if(0 == m_cycReadVector.size())
        {
            return;
        }

        //初始化部分数据结构.
        std::list<AddrInfo> addrList;
        addrList.assign(m_cycReadVector.begin(),m_cycReadVector.end());
        m_curReadIndex = 0;
        m_cycMixedRead.clear();

#ifdef DEBUG_LOG
        for (const auto &var : addrList) {
           LOG(INFO)<<"******begin: "<<var.reg <<" "<<var.index<<" "<<var.len;
        }
#endif
        //1.先排序.
        addrList.sort();

#ifdef DEBUG_LOG
        for (const auto &var : addrList) {
            LOG(INFO)<<"******sort: "<<var.reg<<" "<<var.index<<" "<<var.len;
        }
#endif

        //2.去重+合并.
        static int32_t intervallen = -1;//最大间隔.
        static int32_t maxLen = -1;//最大包长度.
        static std::list<AddrInfo>::iterator itSecnod;
        for (std::list<AddrInfo>::iterator itFirst = addrList.begin(); itFirst != addrList.end();)
        {
            itSecnod = itFirst;
            itSecnod++;
            if(itSecnod == addrList.end())
            {
                break;
            }

            //获取最大间隔和最大长度.
            if(-1 == intervallen)
            {//未给过值
                if(-1 == GetPackageParameters(itFirst->reg,intervallen,maxLen))
                {//不存在该寄存器地址.
                   addrList.erase(itFirst++);
                   continue;
                }
            }
            if(itFirst->reg != itSecnod->reg)
            {//寄存器变了也要更新.
                if(-1 == GetPackageParameters(itSecnod->reg,intervallen,maxLen))
                {//不存在该寄存器地址.
                    addrList.erase(itSecnod);
                    itFirst++;
                    continue;
                }
            }

            //判断是否合并.
            if(m_MDevice && m_MDevice->JudgeAddrBlockMerge(*itFirst,*itSecnod,intervallen,maxLen))
            {//合并到第一个了，删除第二个.
               addrList.erase(itSecnod++);
            }
            else
            {
               ++itFirst;
            }
        }
#ifdef DEBUG_LOG
        for (const auto &var : addrList) {
            LOG(INFO)<<"******deduplication+merging: "<<var.reg<<" "<<var.index<<" "<<var.len;
        }
#endif
        //放入循环读取列表.
        m_cycReadVector.assign(addrList.begin(),addrList.end());

        //开始混合分块处理.
        if(mixedRead == m_protocolReadMode && -1 != m_mixLenMax && -1 != m_mixBlockNumMax)
        {//开始全混合分块.
            static uint32_t totalVarNum;
            static uint32_t mixBlockBeginIndex;
            totalVarNum = 0;
            mixBlockBeginIndex = 0;
            m_cycMixedRead.push_back(0);//开始索引.
            for(uint index = 0; index < m_cycReadVector.size(); ++index)
            {
                totalVarNum += m_cycReadVector.at(index).len;
                if(totalVarNum > static_cast<uint32_t>( m_mixLenMax ) ||
                        (index - mixBlockBeginIndex) >= static_cast<uint32_t>( m_mixBlockNumMax ) )
                {//达到分块条件.
                    m_cycMixedRead.push_back(index);
                    totalVarNum = static_cast<uint32_t>( m_cycReadVector.at(index).len );
                    mixBlockBeginIndex = index;
                }
            }
#ifdef DEBUG_LOG
        for (const auto &var : m_cycMixedRead) {
            LOG(INFO)<<"******MixedRead: "<<var;
        }
#endif
        }
    }
}

int32_t Device::AnalysisFrame(Frame &recvF)
{
    static int32_t ret;
    ret = normal;

    //1.校验帧格式.
    if(m_MDevice)
        ret = m_MDevice->CheckFrame(recvF);
    if(normal != ret)
    {//帧校验失败退出.
        //通讯完成处理.
        //DealCommunicationFinish(m_optFlag,recvF,ret);
        //qDebug()<<__func__<<"CheckFrame:"<<ret;
        return ret;
    }

    //2.解析帧数据.
    switch (m_optFlag) {
    case cycleRead:
    case conditionRead:
    {
       for(std::vector<AddrInfo>::iterator itAddr = recvF.readList.begin();itAddr != recvF.readList.end();++itAddr)
       {
           //qDebug()<<__func__<<i<<QString::fromStdString(recvF.readList.at(i).reg)<<recvF.readList.at(i).index/16<<recvF.readList.at(i).len/16;
           //解析前进行数据特殊处理.
           if(m_MDevice)
           {
               if(-1 == m_MDevice->DealRecvData(*itAddr,recvF.GetRecvBuff()))
               {//子类已经解析完了,下一个地址块.
                   continue;
               }
           }
           //开始解析.
           for(std::vector<Protocol::AddrInfoForRW>::iterator it = itAddr->varList.begin();
               it != itAddr->varList.end();++ it)
           {
               ret = m_DataAreaDeal.SetDataFromAddr(it->reg,
                                                    it->index,
                                                    it->len,
                                                    recvF.GetRecvBuff() + GetBeginIndex(*it,*itAddr),
                                                    static_cast<uint16_t>( GetBitOffset(*it,*itAddr)) );
           }
       }
    }
        break;
    case write:
    {
        //无需解析数据.
    }
        break;
    }

    //通讯完成处理.
    //DealCommunicationFinish(m_optFlag,recvF,0);

    return ret;
}

void Device::DealCommunicationFinish(int32_t optFlag,const Frame &curFrame, int32_t result)
{
    switch (optFlag) {
    case cycleRead:
    {
        if(continuousRead ==  m_protocolReadMode)
        {//全连续读.
            m_curReadIndex++;
            if(m_curReadIndex >= m_cycReadVector.size())
            {
                m_curReadIndex = 0;
            }
        }
        else
        {//全混合读.
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
            if( 0 < curFrame.readList.size() && 0 < curFrame.readList.front().varList.size() &&
                    curFrame.readList.front().varList.front().callback)
            {//条件异步读取回调.
               curFrame.readList.front().varList.front().callback(m_conditonReadList.front(),result);
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
            if( 0 < curFrame.writeList.size() && curFrame.writeList.front().callback)
            {//写入回调.
               curFrame.writeList.front().callback(m_writeList.front(),result);
            }
            m_writeList.pop_front();
        }
        m_writeLock.unlock();
    }
        break;
    }
}

int32_t Device::BuildSendFrame(std::vector<Protocol::AddrInfoForRW> &addrVec)
{
    std::vector<AddrInfo> addrinfoVec;
    int32_t ret = 0;
    std::vector<Protocol::AddrInfoForRW> addrInfo;

    //先处理全局帧（握手注册帧）.
    if(m_MDevice)
    {
        m_MDevice->GetGlobalFrame();
    }

    //处理数据读写帧.
    switch (m_optFlag) {
    case cycleRead:
    {
        if(m_cycReadLock.try_lock())
        {//如果获取到锁了就往下，循环读设置阻塞了就退出下一次，优先级让给条件读和写入操作.
            if(0 == m_cycReadVector.size())
            {//检查是否有循环读项.
                m_cycReadLock.unlock();
                return 0;
            }
            addrVec.clear();
            if(continuousRead ==  m_protocolReadMode)
            {//全连续读.
                if(m_curReadIndex < m_cycReadVector.size())
                {
                    addrVec = m_cycReadVector.at(m_curReadIndex).varList;//当前读取的变量列表.
                    addrinfoVec.emplace_back( m_cycReadVector.at(m_curReadIndex) );//当前读取的地址信息.
                }
            }
            else
            {//全混合读.
                if(m_curReadIndex < m_cycMixedRead.size())
                {
                    if(m_curReadIndex == m_cycMixedRead.size()-1)
                    {//最后一组.
                        for (uint32_t i = m_cycMixedRead.at(m_curReadIndex) ; i < m_cycReadVector.size(); ++i)
                        {
                            addrVec.insert(addrVec.end(),
                                           m_cycReadVector.at(i).varList.begin(),
                                           m_cycReadVector.at(i).varList.end());
                            addrinfoVec.emplace_back(m_cycReadVector.at(i));
                        }
                    }
                    else
                    {
                        for (uint32_t i = m_cycMixedRead.at(m_curReadIndex) ; i < m_cycReadVector.size() && i < m_cycMixedRead.at(m_curReadIndex+1); ++i)
                        {
                            addrVec.insert(addrVec.end(),
                                           m_cycReadVector.at(i).varList.begin(),
                                           m_cycReadVector.at(i).varList.end());
                            addrinfoVec.emplace_back(m_cycReadVector.at(i));
                        }
                    }
                }
            }

            //处理是否开启了循环读取帧缓存模式.
            if(m_bCycReadFrameCache)
            {//循环读帧缓存开启.
                std::map<uint32_t,std::deque<Frame> >::iterator itF = m_cycFrameMapOfCurReadIndex.find(m_curReadIndex);
                if(itF != m_cycFrameMapOfCurReadIndex.end())
                {//有.
                    m_curFrameList.insert(m_curFrameList.end(),itF->second.begin(),itF->second.end());
                }
                else
                {//没有.
                    if(m_MDevice)
                    {
                        ret = m_MDevice->ReadFrame(addrinfoVec);
                    }
                    if(0 < m_curFrameList.size() )
                    {
                        //筛选掉全局帧.
                        std::deque<Frame> curFrame;
                        foreach (auto frame, m_curFrameList) {
                            if(!frame.bGlobalFrame){
                                curFrame.emplace_back(frame);
                            }
                        }
                        //加入循环读帧缓存.
                        m_cycFrameMapOfCurReadIndex[m_curReadIndex] = curFrame;
                    }
                }
            }
            else
            {
                if(m_MDevice)
                {
                    ret = m_MDevice->ReadFrame(addrinfoVec);
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
        {//检查是否为空.
            m_conditionReadLock.unlock();
            return 0;
        }
        if(0 == m_syncRWFlag)
        {//异步从头.
            addrVec.push_back(m_conditonReadList.front());
            AddrInfo addrinfo(m_conditonReadList.front().reg,m_conditonReadList.front().index,m_conditonReadList.front().len);
            addrinfo.varList.emplace_back(m_conditonReadList.front());
            addrinfoVec.emplace_back(addrinfo);
            if(m_MDevice)
                ret = m_MDevice->ReadFrame(addrinfoVec);
        }
        else
        {//同步从后面拿.
            addrVec.push_back(m_conditonReadList.back());
            AddrInfo addrinfo(m_conditonReadList.front().reg,m_conditonReadList.front().index,m_conditonReadList.front().len);
            addrinfo.varList.emplace_back(m_conditonReadList.front());
            addrinfoVec.emplace_back(addrinfo);
            if(m_MDevice)
                ret = m_MDevice->ReadFrame(addrinfoVec);
        }
        m_conditionReadLock.unlock();
    }
        break;
    case write:
    {
        addrInfo.clear();
        m_writeLock.lock();
        if(0 == m_writeList.size())
        {//检查是否为空.
            m_writeLock.unlock();
            return 0;
        }
        if(0 == m_syncRWFlag)
        {//异步从头.
            addrInfo.push_back(m_writeList.front());
            addrVec.clear();
            addrVec.push_back(m_writeList.front());
        }
        else
        {//同步从后面拿.
            addrInfo.push_back(m_writeList.back());
            addrVec.clear();
            addrVec.push_back(m_writeList.back());
        }
        m_writeLock.unlock();
        if(m_MDevice)
            ret = m_MDevice->WriteFrame(addrInfo);
    }
        break;
    }
    return ret;
}

int32_t Device::GetLastData(AddrInfoForRW &addr)
{
    int32_t ret = Protocol_Rtn_Success;
    //获取寄存器信息.
    Protocol::RegInfo rInfo;
    if(Protocol_Rtn_Fail == GetRefInfoFromReg(addr.reg,rInfo))
    {
        return Protocol_Rtn_Fail;
    }
    if(Protocol::WriteOnly != rInfo.rwFlag)
    {//非只写
        //标签-数据类型字符串处理长度.
        if(0 == addr.len)
        {//标签字符串没有长度，读取时确定.
            Protocol::RegInfo reginfo;
            if(Protocol_Rtn_Success == GetRefInfoFromReg(addr.reg,reginfo))
            {
                addr.len = reginfo.unitBitNum;
            }
        }
        //读值.
        //if(Protocol::string != addr.dataType)
        {
            //开始.
            uint64_t len = addr.len;
            //初始化buf.
            std::vector<char> bufv;
            if(Protocol::string == addr.dataType)
            {
                int32_t iUnit = GetRegUint(addr.reg);
                if(Protocol_Rtn_Fail != iUnit)
                    bufv.resize(len*(static_cast<uint32_t>(iUnit)/8));
                else
                    bufv.resize(len*8);
            }
            else
            {
                bufv.resize(len*8);//最大的数据类型是8字节
            }
            //一次性获取数据.
            if(-1 == m_DataAreaDeal.GetDataFromAddr(addr.reg,addr.index,len,bufv.data()))
            {
                ret = Protocol_Rtn_Fail;
            }
            else
            {
                if(Protocol::bit != addr.dataType)
                {//字.
                    //处理字节序.
                    if(m_MDevice)
                        m_MDevice->DealByteOrder(bufv.data(),addr.dataType);
                }
                //数据转换.
                Char2Variant(addr.dataType,bufv,addr.value);
            }
        }
    }
    return ret;
}

int32_t Device::SetLastData(Protocol::AddrInfoForRW &addr)
{
    #ifdef DEBUG_LOG
    LOG(INFO)<<"******"<<__func__<<":"<<addr.reg<<" "<<addr.index<<" "<<addr.len<<" "<<addr.dataType;
    #endif
    int32_t ret = Protocol_Rtn_Success;

    //获取寄存器信息.
    Protocol::RegInfo rInfo;
    if(Protocol_Rtn_Fail == GetRefInfoFromReg(addr.reg,rInfo))
    {
        return Protocol_Rtn_Fail;
    }

    if(Protocol::WriteOnly != rInfo.rwFlag)
    {//非只写类型才有地址缓存.
        //开始.
        uint64_t len = addr.len;
        //初始化buf.
        std::vector<char> bufv;
        if(Protocol::string == addr.dataType) bufv.resize(len*8);
        else bufv.resize(len*8);//最大的数据类型是8字节
        //数据转换.
        Variant2Char(addr.value,bufv.data());
        //判断字和位.
        if(Protocol::bit != addr.dataType)
        {//字.
            //字节序.
            if(m_MDevice)
                m_MDevice->DealByteOrder(bufv.data(),addr.dataType);
        }
        //数据写入.
        if(-1 == m_DataAreaDeal.SetDataFromAddr(addr.reg,addr.index,len,bufv.data(),0))
        {
            ret = Protocol_Rtn_Fail;
        }
    }
    return ret;
}

int32_t Device::GetDataFromAddrAndDatatype(const std::string &reg, const uint64_t &index, const Protocol_DataType &dataType, DataVariant &data)
{
    Protocol::AddrInfoForRW addr;
    addr.reg = reg;
    addr.index = index;
    addr.dataType = dataType;
    GetLastData(addr);
    if( DataVariant_Null != addr.value.index() )
    {
        data = addr.value;
        return 0;
    }
    //填充长度
    GetBitNumFromDatatype(addr.reg,addr.dataType,addr.len);
    return -1;
}

int32_t Device::SetDataFromAddrAndDatatype(const std::string &reg, const uint64_t &index, const Protocol_DataType &dataType, const DataVariant &data)
{
    Protocol::AddrInfoForRW addr;
    addr.reg = reg;
    addr.index = index;
    addr.dataType = dataType;
    addr.value = data;
    if(string == dataType)
    {//字符串计算长度.
        addr.len = std::get<std::string>(data).size();
    }
    //填充长度
    GetBitNumFromDatatype(addr.reg,addr.dataType,addr.len);
    return SetLastData(addr);
}

void Device::ClearCycReadCacheFrame()
{
    m_cycFrameMapOfCurReadIndex.clear();
}

int32_t Device::GetVersion(std::string &sVersion)
{
    if(m_MDevice)
    {
        return m_MDevice->GetVersion(sVersion);
    }
    return -1;
}

int32_t Device::Process(std::vector<AddrInfoForRW> &addrList)
{//同步.
    static int ret;
    ret = 0;
    //组帧.
    if(0 == m_syncRWFlag)
    {//异步按优先级来.
        m_writeLock.lock();
        static uint64_t writeNum;
        writeNum = m_writeList.size();
        m_writeLock.unlock();

        m_conditionReadLock.lock();
        static uint64_t condReadeNum;
        condReadeNum = m_conditonReadList.size();
        m_conditionReadLock.unlock();

        m_cycReadLock.lock();
        static uint64_t cycReadNum;
        cycReadNum = m_cycReadVector.size();
        m_cycReadLock.unlock();

        if(0 < writeNum && m_writeTimes <= m_deviceConfig.protocolInfo.RWThreshold)
        {//连续写入.
            m_writeTimes++;
            m_optFlag = Device::write;
            BuildSendFrame(addrList);
        }
        else if (0 < condReadeNum && m_conditonReadTimes <= m_deviceConfig.protocolInfo.RWThreshold)
        {//条件读.
            m_writeTimes = 0;
            m_conditonReadTimes++;
            m_optFlag = Device::conditionRead;
            BuildSendFrame(addrList);
        }
        else if(0 < cycReadNum )
        {//循环读.
            m_conditonReadTimes = 0;
            m_optFlag = Device::cycleRead;
            BuildSendFrame(addrList);
        }
    }
    else
    {//同步读写.
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

    #ifdef DEBUG_LOG
    if(0 < m_curFrameList.size())
    {
        LOG(INFO)<<"*****Process "<<m_optFlag;
        foreach (auto addr, addrList) {
            LOG(INFO)<<"***"<<addr.reg<<" "<<addr.index<<" "<<addr.len<<" "<<addr.dataType;
        }
        LOG(INFO)<<"************";
    }
    #endif

    //开始收发数据.
    if( m_comm && 0 < m_curFrameList.size() )
    {
        //先取出回调.
        //一轮通讯中，以最后一帧的回调为主.
        static Frame frame_back;
        frame_back = m_curFrameList.back();

        //开始处理数据帧.
        while (0 != m_curFrameList.size())
        {
            //一次通讯前处理.
            if(m_MDevice)
            {
                m_MDevice->BeforeCommunication();
            }
            //发送.
            ret = m_comm->SendData(m_curFrameList.front().GetSendBuff(), m_curFrameList.front().sendLen);
            #ifdef DEBUG_LOG
            LOG(INFO)<<"******"<<"Send:"<<m_curFrameList.front().sendLen<<" "<<ret;
            #endif
            if(ret < 0)
            {//重连.
                m_comm.get()->CloseComm();
                ushort ret =  static_cast<ushort>( m_comm.get()->OpenComm() );
                //if(0 != ret)
                {//连接失败.
                    SetDataFromAddrAndDatatype(Protocol_Status_Reg_Name,Protocol_Status_CommStatus,Protocol::ubit16,ret);
                }
            }
            //开始接收处理数据.
            ret = RecvData();
            //更新通讯结果到状态寄存器,并判断异常日志写入.
            UpdataProcessResultToDataArea(ret,addrList);
            //处理结果.
            if(Protocol_Process_R_Suc != ret && Protocol_Process_W_Suc != ret && Protocol_Process_Retry != ret)
            {//通信（重试过）不成功,直接清掉当前发送队列.
                m_curFrameList.clear();
                break;
            }
        }
        //通讯完成处理.
        if(Protocol_Process_R_Suc == ret || Protocol_Process_W_Suc == ret)
        {//通讯成功.
            DealCommunicationFinish(m_optFlag,frame_back,0);
        }
        else
        {//通讯失败.
            DealCommunicationFinish(m_optFlag,frame_back,-1);
        }
    }
    return ret;
}

//从站模式下数据回调.
//返回值：接收的数据处理了多少，单位字节.
int Device::Slave_ReadReady_CB(int code, char *buf, uint32_t len,char **sBuf, uint32_t *sLen)
{
    if(0 == code && 0 < len )
    {//有数据.
        if(m_MDevice)
        {
            m_MDevice->DealDataOfSlave(buf,len,sBuf,sLen);
        }
    }
    //处理完成.
    return static_cast<int32_t>(len);
}

//标签处理转换.
int32_t Device::TagDeal(AddrInfoForRW &addr)
{
    int32_t ret = Protocol_Rtn_Success;
#ifdef TAG_MODE
    //协议标签模式.
    //补全一些信息.
    RegInfo regInfo;
    if(Protocol_Rtn_Success == GetRefInfoFromReg(addr.reg,regInfo)
            && Protocol::null_dType != regInfo.dataType)
    {
        addr.index = 0;
        addr.dataType = regInfo.dataType;
        addr.len = 0;
    }
    ret = Protocol_Rtn_Success;
#else
//标签别名.
    std::map<std::string,Protocol::Tag_Info>::iterator it =  m_tagInfoMap.find(addr.reg);
    if(it != m_tagInfoMap.end())
    {//标签别名.
       addr.reg = it->second.addrInfo.reg;
       addr.index = it->second.addrInfo.index;
       addr.dataType = it->second.dataType;
       ret = Protocol_Rtn_Success;
    }
    ret = Protocol_Rtn_Fail;
#endif

#ifdef DEBUG_LOG
LOG(INFO)<<"******"<<__func__<<" :"<<addr.reg<<" "<<addr.index<<" "<<addr.len<<" "<<addr.dataType;
#endif

    //处理一下字节对齐.
    //CompleteByByte(addr.index,addr.len);

    //通过数据类型转换长度
    GetBitNumFromDatatype(addr.reg,addr.dataType,addr.len);

    return ret;
}


void Device::GetBitNumFromDatatype(const std::string &reg,Protocol_DataType dataType, uint64_t &bitNum)
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
        bitNum = bitNum*8;
    }
        break;
    default:
    {
        //
    }
    }

    //转换
    int32_t iUnit = GetRegUint(reg);

    if(-1 != iUnit) bitNum = bitNum/static_cast<uint32_t>( iUnit );
    if(0 == bitNum) bitNum = 1;
}

void Device::InitFrame(Frame &frame)
{
    frame.send.clear();
    frame.recv.clear();
    frame.readList.clear();
    frame.writeList.clear();
    frame.bGlobalFrame = false;
}

void Device::AddCommStatusRegInfo()
{
    m_regInfoMap.emplace(Protocol_Status_Reg_Name,RegInfo{Protocol_Status_Reg_Name,"DDDD",255,0,readWrite,16});
}

void Device::UpdataProcessResultToDataArea(int32_t ret, const std::vector<AddrInfoForRW> &addrList)
{
    static int32_t oldRet = 0;
    if(oldRet == ret){return;}
    else {oldRet = ret;}

    SetDataFromAddrAndDatatype(Protocol_Status_Reg_Name,Protocol_Status_ErrCode,Protocol::ubit16,ushort(ret));
    switch (ret) {
    case Protocol_Process_R_NoData:
    case Protocol_Process_W_NoData:
    {
        if(m_deviceConfig.commInfo.isOpenErrLog)
        {
           QString sErr;
           if(Protocol_Process_R_NoData == ret)
           {
             sErr += QString::fromLocal8Bit("读:\n");
           }
           else
           {
             sErr += QString::fromLocal8Bit("写:\n");
           }
           foreach (auto addr, addrList) {
           #ifndef TAG_MODE
               sErr += QString("%1 %2 %3 %4\n").arg(addr.linkName.data()).arg(addr.reg.data()).arg(addr.index).arg(addr.len);
           #else
               sErr += QString::fromUtf8( addr.linkName.data() ) +":"+ QString::fromUtf8( addr.reg.data() ) + "\n";
           #endif
           }
           sErr += QString::fromLocal8Bit("\n结果:通讯无数据响应");
           //IAPP::GetIntance()->Log(IAPP::Log_Error_Slight,"communication error",sErr.toLocal8Bit().data());
        }
    }
        break;
    case Protocol_Process_R_CheckError:
    case Protocol_Process_W_CheckError:
    {
        if(m_deviceConfig.commInfo.isOpenErrLog)
        {
            QString sErr;
            if(Protocol_Process_R_NoData == ret)
            {
              sErr += QString::fromLocal8Bit("读:\n");
            }
            else
            {
              sErr += QString::fromLocal8Bit("写:\n");
            }
            foreach (auto addr, addrList) {
            #ifndef TAG_MODE
                sErr += QString("%1 %2 %3 %4\n").arg(addr.linkName.data()).arg(addr.reg.data()).arg(addr.index).arg(addr.len);
            #else
                sErr += QString::fromUtf8( addr.linkName.data() ) +":"+ QString::fromUtf8( addr.reg.data() ) + "\n";
            #endif
            }
            sErr += QString::fromLocal8Bit("\n结果:通讯响应数据校验失败");
            //IAPP::GetIntance()->Log(IAPP::Log_Error_Slight,"communication error",sErr.toLocal8Bit().data());
        }
    }
        break;
    }
}

uint64_t Device::GetBeginIndex(const AddrInfoForRW &addrRw, const AddrInfo &addr)
{
    RegInfo rInfo;
    if(-1 == GetRefInfoFromReg(addr.reg,rInfo))
    {
        rInfo.unitBitNum = 16;
    }
    return  addr.beginIndex +  static_cast<uint64_t>((addrRw.index - addr.index)/(8.0/rInfo.unitBitNum));
}

uint64_t Device::GetBitOffset(const AddrInfoForRW &addrRw, const AddrInfo &addr)
{
    RegInfo rInfo;
    if(-1 == GetRefInfoFromReg(addr.reg,rInfo))
    {
        rInfo.unitBitNum = 16;
    }
    if(1 == rInfo.unitBitNum)
        return (addrRw.index - addr.index)%(8);
    else
        return 0;

}

int32_t Device::Process_Slave()
{//同步.
    static int ret;
    ret = 0;
    if( m_comm )
    {
        //主动收发模式和回调模式.
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
           //设置回调.
           m_comm->OpenComm(0,500,nullptr,nullptr,
                            std::bind(&Device::Slave_ReadReady_CB,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,
                                                            std::placeholders::_4,std::placeholders::_5));
           //启动事件循环.
           m_comm->AsynProcess();
        }
    }
    return ret;
}

int32_t Device::Process_All()
{
    static int32_t ret;
    static std::vector<AddrInfoForRW> addrList;

    if(m_threadIsRun && m_deviceConfig.commInfo.isOnline)
    {//运行标识符和在线模式下.
        m_commLock.lock();
        ret = 0;
        addrList.clear();
        if(Master == m_deviceConfig.protocolInfo.masterOrSlave)
        {//主站模式.
            ret = Process(addrList);
        }
        else if(Slave == m_deviceConfig.protocolInfo.masterOrSlave)
        {//从站模式.
           ret = Process_Slave();
        }
        else
        {//混合模式.
          //先检查有无推送的请求.
          Process_Slave();
          //再主动请求.
          ret = Process(addrList);
        }
        m_commLock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(m_deviceConfig.protocolInfo.communicationInterval));
    }
    else
    {//暂停.
        std::this_thread::sleep_for(std::chrono::milliseconds(m_deviceConfig.protocolInfo.timeout));
    }
    return ret;
}

void Device::Run()
{//线程run.
    m_threadIsOver = false;
    while (m_bThreadFlag)
    {
        //处理主从通讯.
        Process_All();
    }
    m_threadIsOver = true;
}

void Device::StopThread()
{
    if(m_processThread)
    {//关闭线程.
        m_bThreadFlag = false;
        m_threadIsRun = false;
        m_processThread->join();
        m_processThread.reset();
    }
}
