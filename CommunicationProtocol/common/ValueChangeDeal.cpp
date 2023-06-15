#include "ValueChangeDeal.h"
#include "service.h"
#include "typedefheader.h"
#include <QtGlobal>

#include "glog/logging.h"

ValueChangeDeal::ValueChangeDeal(DataAreaDeal *dataAreaDeal):m_DataAreaDeal(dataAreaDeal)
{
}

void ValueChangeDeal::DealAddrValueChangeCB(const std::string &reg, uint64_t index, int32_t ret)
{
    m_valueChangeLock.lock();
    //协议标签模式.
    //处理值变化回调.
    Protocol::AddrInfoForRW ACD("",reg,index);
    std::pair<std::multiset<Protocol::AddrInfoForRW>::iterator, std::multiset<Protocol::AddrInfoForRW>::iterator> it;
    it = m_valueChangeAddrInfoSet.equal_range(ACD);
    for (std::multiset<Protocol::AddrInfoForRW>::iterator findIt = it.first;findIt != it.second;++findIt)
    {
        if(findIt->callback)
        {
            findIt->callback(*findIt,ret);
        }
    }
    m_valueChangeLock.unlock();
}

int32_t ValueChangeDeal::SetRegValueChangeCallback(std::vector<Protocol::AddrInfoForRW> &addrVec, Protocol::Add_Type addType)
{
    m_valueChangeLock.lock();
    //协议标签模式
    //添加类型判断.
    if(Protocol::Cover == addType)
    {
        m_valueChangeAddrInfoSet.clear();
    }

    for (uint32_t var = 0; var < addrVec.size(); ++var) {
        //标签别名转换.
        Device::GetInstance().TagDeal(addrVec.at(var));
        //添加.
        m_valueChangeAddrInfoSet.emplace(addrVec.at(var));
    }
    m_valueChangeLock.unlock();

    return 0;
}
