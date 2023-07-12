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

    //获取地址缓存中的数据.
    //len的单位是位
    int32_t GetDataFromAddr(const std::string &reg, uint64_t index, uint64_t len ,char * data, uint16_t bitOffset = 0);

    //设置地址缓存数据.
    //dataLen的单位是位
    int32_t SetDataFromAddr(const std::string &reg, uint64_t index, uint64_t dataLen ,char * data, uint16_t bitOffset = 0);

    //设置地址变化回调信息.
    int32_t SetRegValueChangeCallback(std::vector<Protocol::AddrInfoForRW> &addrVec,
                                      Protocol::Add_Type addType);

    //读位，写位
    char GetBit(char *byte, uint32_t num)
    {
        if(((*(byte+(num>>3)))>>(num%8))&0x1)
        {
            return 1;
        }
        else {
            return 0;
        }
    }

    bool SetBit(char *byte, uint32_t num, char value)
    {
        if(value == GetBit(byte,num))
        {//无需设置值
            return false;
        }
        if(!value)
        {
            *byte =  (~(1<<(num%8)))&(*(byte+(num >>3)));
        }
        else
        {
            *byte =  (1<<(num%8))|(*(byte+(num >>3)));
        }
        return true;
    }

protected:
    std::set<Data_Area>::iterator GetDataAreaIterator(const std::string &reg , uint64_t index);


private:
    //地址缓冲区.
    std::set<Data_Area> m_dataArea;

    std::mutex m_dataAreaLock;

    ValueChangeDeal *m_ValueChangeDeal = nullptr;
};

#endif // DATAAREADEAL_H
