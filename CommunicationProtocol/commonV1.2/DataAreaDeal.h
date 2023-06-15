#ifndef DATAAREADEAL_H
#define DATAAREADEAL_H

#include "service_def.h"
#include <set>
#include <mutex>
#include "ValueChangeDeal.h"

class DataAreaDeal
{
public:
    DataAreaDeal();
    virtual ~DataAreaDeal();

    std::set<Data_Area>::iterator GetDataAreaIterator(Data_Area &dataA);

    //获取地址缓存中的数据.
    int32_t GetDataFromAddr(const std::string &reg, uint64_t index, uint64_t len, char * data);

    //设置地址缓存数据.
    int32_t SetDataFromAddr(const AddrInfo &addr, char * data);

    //设置地址变化回调信息.
    int32_t SetRegValueChangeCallback(std::vector<Protocol::AddrInfoForRW> &addrVec,
                                      Protocol::Add_Type addType);


private:
    //地址缓冲区.
    std::set<Data_Area> m_dataArea;

    std::mutex m_dataAreaLock;

    ValueChangeDeal *m_ValueChangeDeal = nullptr;
};

#endif // DATAAREADEAL_H
