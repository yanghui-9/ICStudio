#ifndef VALUECHANGEDEAL_H
#define VALUECHANGEDEAL_H
#include "service_def.h"
#include <map>
#include <set>
#include <mutex>
#include <vector>
#include <stddef.h>

class Device;
class DataAreaDeal;
class ValueChangeDeal
{
public:
    ValueChangeDeal(DataAreaDeal *dataAreaDeal);

    //处理地址值变化回调.
    void DealAddrValueChangeCB(const AddrInfo &addr, int32_t ret);
    //设置地址变化回调信息.
    int32_t SetRegValueChangeCallback(std::vector<Protocol::AddrInfoForRW> &addrVec,
                                      Protocol::Add_Type addType);

private:
    //值变化回调.
#ifdef TAG_MODE
    //标签协议模式
    std::multiset<Addr_Change_Data> m_valueChangeAddrInfo;
#else
    //std::multimap<uint64_t,Addr_Change_Data> m_valueChangeAddrInfoMap;
    //std::vector<Addr_Change_Data> m_valueChangeAddrInfo;
    std::map<std::string,std::vector<Addr_Change_Data>> m_valueChangeAddrInfo;
#endif
    //值变化信息锁.
    std::mutex m_valueChangeLock;

    DataAreaDeal * m_DataAreaDeal = nullptr;
};

#endif // VALUECHANGEDEAL_H
