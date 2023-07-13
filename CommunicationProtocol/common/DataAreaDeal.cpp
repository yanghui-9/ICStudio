#include "DataAreaDeal.h"
#include <glog/logging.h>
#include "typedefheader.h"
#include "service.h"

DataAreaDeal::DataAreaDeal()
{
    m_ValueChangeDeal = new ValueChangeDeal(this);
}

DataAreaDeal::~DataAreaDeal()
{
    if(m_ValueChangeDeal)
    {
        delete m_ValueChangeDeal;
        m_ValueChangeDeal = nullptr;
    }
}

std::set<Data_Area>::iterator DataAreaDeal::GetDataAreaIterator(const std::string &reg, uint64_t index)
{
    Data_Area dataA;
    dataA.reg = reg;
    dataA.index = index;
    std::set<Data_Area>::iterator it =  m_dataArea.find(dataA);
    if(it == m_dataArea.end())
    {
        //先初始化地址块再插入.
        dataA.index = index;
        memset(dataA.data,0,ADDRESS_AREA_UNIT);
        m_dataAreaLock.lock();//插入锁.
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

int32_t DataAreaDeal::GetDataFromAddr(const std::string &reg, uint64_t index, uint64_t len, char *data, uint16_t bitOffset)
{
#ifdef DEBUG_LOG
    LOG(INFO)<<"******"<<__func__<<":"<<reg<<index<<" "<<len;
#endif
   //获取寄存器基本单位 & dataLen转换为位单位
   int32_t iUnit = Device::GetInstance().GetRegUint(reg);
   if(-1 == iUnit)
   {
       return -1;
   }
   len = len * static_cast<uint32_t>(iUnit);

   //查询该地址变量
   std::set<Data_Area>::iterator it = GetDataAreaIterator(reg,index);
   if(it == m_dataArea.end())
   {
       return -1;
   }
   char *src = const_cast<char *>( it->data );
   if(0 == bitOffset%8)
   {//没有位偏移
       uint64_t times = len >> 3;
       uint64_t remainder = len % 8;
       memcpy(data+(bitOffset>>3),src,times);
       for(uint32_t i = 0; i < remainder; ++i)
       {
           SetBit(data+(bitOffset>>3)+times,i,GetBit(src+times,i));
       }
   }
   else
   {
       for(uint32_t i = 0; i < len ; ++i)
       {
           SetBit(data,i+bitOffset,GetBit(src,i));
       }
   }
   return 0;
}

int32_t DataAreaDeal::SetDataFromAddr(const std::string &reg, uint64_t index, uint64_t dataLen, char *data, uint16_t bitOffset)
{
#ifdef DEBUG_LOG
    LOG(INFO)<<"******"<<__func__<<":"<<reg<<index<<" "<<dataLen;
#endif
    //获取寄存器基本单位 & dataLen转换为位单位
   int32_t iUnit = Device::GetInstance().GetRegUint(reg);
   if(-1 == iUnit)
   {
       return -1;
   }
   dataLen = dataLen * static_cast<uint32_t>(iUnit);

   //查询该地址变量
   std::set<Data_Area>::iterator it = GetDataAreaIterator(reg,index);
   if(it == m_dataArea.end())
   {
       return -1;
   }
   //resize(dataLen%8 == 0 ?dataLen/8:dataLen/8+1);
   char *dst = const_cast<char *>( it->data);

   //开始设置值
   bool valueChangeFlag = false;
   if(0 == bitOffset%8)
   {//没有位偏移
       uint64_t times = dataLen >> 3;
       uint64_t remainder = dataLen % 8;
       if(0 != times)
       {
           if(0 != Strcmp(dst,data+(bitOffset>>3),static_cast<uint32_t>( times )))
           {
               valueChangeFlag = true;
           }
           memcpy(dst,data+(bitOffset>>3),times);
       }
       for(uint32_t i = 0; i < remainder; ++i)
       {
          if(SetBit(dst+times,i,GetBit(data+(bitOffset>>3)+times,i)))
          {
              valueChangeFlag = true;
          }
       }
   }
   else
   {
       for(uint32_t i = 0; i < dataLen ; ++i)
       {
          if(SetBit(dst,i,GetBit(data,i+bitOffset)))
          {
             valueChangeFlag = true;
          }
       }
   }
   //值变化回调
   if(valueChangeFlag)
   {
       m_ValueChangeDeal->DealAddrValueChangeCB(reg,index,0);
   }

   return 0;
}

int32_t DataAreaDeal::SetRegValueChangeCallback(std::vector<Protocol::AddrInfoForRW> &addrVec, Protocol::Add_Type addType)
{
    return m_ValueChangeDeal->SetRegValueChangeCallback(addrVec,addType);
}

