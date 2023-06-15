#include "ValueChangeDeal.h"
#include "service.h"
#include "typedefheader.h"
#include <QtGlobal>

ValueChangeDeal::ValueChangeDeal(DataAreaDeal *dataAreaDeal):m_DataAreaDeal(dataAreaDeal)
{
}

void ValueChangeDeal::DealAddrValueChangeCB(const AddrInfo &addr, int32_t ret)
{
#ifndef TAG_MODE
    //非协议标签模式.
    //处理值变化回调.
    m_valueChangeLock.lock();

    std::map<std::string,std::vector<Addr_Change_Data>>::iterator regFindIt = m_valueChangeAddrInfo.find(addr.reg);
    if(regFindIt != m_valueChangeAddrInfo.end())
    {
        Addr_Change_Data ACD;
        ACD.valueChangeAddrInfo.reg = addr.reg;
        ACD.valueChangeAddrInfo.index = addr.index+addr.len-1;
        ACD.valueChangeAddrInfo.len = addr.len;
        std::vector<Addr_Change_Data>::iterator itF = std::upper_bound(regFindIt->second.begin(),regFindIt->second.end(),ACD);
        for (std::vector<Addr_Change_Data>::iterator itF0 = regFindIt->second.begin(); itF0 != itF; itF0++)
        {
            if(itF0->valueChangeAddrInfo.index + itF0->valueChangeAddrInfo.len -1 >= addr.index)
            {//判断是否重叠.
                uint32_t dataLen = static_cast<uint32_t>( itF0->valueChangeAddrInfo.len/8 );//算出字节长度
                char * datanow = const_cast<char*>( itF0->valueChangeAddrInfoBuffNew.data());
                char * data = const_cast<char*>( itF0->valueChangeAddrInfoBuff.data());
                m_DataAreaDeal->GetDataFromAddr(itF0->valueChangeAddrInfo.reg,
                                                itF0->valueChangeAddrInfo.index,
                                                static_cast<uint32_t>( itF0->valueChangeAddrInfo.len ),
                                                datanow);
                if(0 != Strcmp(data,datanow,dataLen))
                {//不等.
                    //qDebug()<<"read:"<<QString::fromStdString(addrInfo.reg)<<addrInfo.index/16<<addrInfo.len/16;
                    //qDebug()<<timesss<<QString::fromStdString( it->reg )<<it->index/16<<it->len/16<<dataLen<<(int)data[0]<<(int)data[1]<<(int)datanow[0]<<(int)datanow[1];
                    memcpy(data,datanow,dataLen);
                    //m_valueChangeCallback(findIt->valueChangeAddrInfo,ret);
                    if(itF0->valueChangeAddrInfo.callback)
                    {
                        itF0->valueChangeAddrInfo.callback(itF0->valueChangeAddrInfo,ret);
                    }
                }
            }
        }
    }
    m_valueChangeLock.unlock();

#else
    m_valueChangeLock.lock();

    foreach (auto addr,addr.varList) {
        //协议标签模式.
        //处理值变化回调.
        Addr_Change_Data ACD(addr);
        std::pair<std::multiset<Addr_Change_Data>::iterator, std::multiset<Addr_Change_Data>::iterator> it;
        it = m_valueChangeAddrInfo.equal_range(ACD);
        for (std::multiset<Addr_Change_Data>::iterator findIt = it.first;findIt != it.second;++findIt)
        {
            if(findIt->valueChangeAddrInfo.callback)
            {
                findIt->valueChangeAddrInfo.callback(findIt->valueChangeAddrInfo,ret);
            }
        }
    }


    m_valueChangeLock.unlock();
#endif
}

int32_t ValueChangeDeal::SetRegValueChangeCallback(std::vector<Protocol::AddrInfoForRW> &addrVec, Protocol::Add_Type addType)
{
    m_valueChangeLock.lock();

#ifdef TAG_MODE
    //协议标签模式
    //添加类型判断.
    if(Protocol::Cover == addType)
    {
        m_valueChangeAddrInfo.clear();
    }

    for (uint32_t var = 0; var < addrVec.size(); ++var) {
        //标签别名转换.
        Device::GetInstance().TagDeal(addrVec.at(var));
        //添加.
        m_valueChangeAddrInfo.emplace(addrVec.at(var));
    }
#else
    //添加类型判断.
    if(Protocol::Cover == addType)
    {
        m_valueChangeAddrInfo.clear();
    }
    //排序
    std::sort(addrVec.begin(),addrVec.end());
    //插入
    std::vector<Addr_Change_Data> addrData;
    Addr_Change_Data tmpData;
    for (uint32_t var = 0; var < addrVec.size(); ++var) {
        //标签别名转换.
        Device::GetInstance().TagDeal(addrVec.at(var));
        //添加.
        if(0 == addrData.size())
        {
           addrData.emplace_back( addrVec.at(var) );
        }
        else
        {
           if(addrData.front().valueChangeAddrInfo.reg == addrVec.at(var).reg)
           {//寄存器相等.
               addrData.emplace_back(addrVec.at(var));
           }
           else
           {//不相等.
               m_valueChangeAddrInfo.emplace(addrData.front().valueChangeAddrInfo.reg,addrData);
               addrData.clear();
               addrData.emplace_back(addrVec.at(var));
           }
        }
        if(var == addrVec.size()-1)
        {//最后
           if(0 < addrData.size())
           {
             m_valueChangeAddrInfo.emplace(addrData.front().valueChangeAddrInfo.reg,addrData);
           }
        }
    }
#endif

    m_valueChangeLock.unlock();
    return 0;
}
