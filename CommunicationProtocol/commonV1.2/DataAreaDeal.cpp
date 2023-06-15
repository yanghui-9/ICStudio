#include "DataAreaDeal.h"
#include <glog/logging.h>
#include "typedefheader.h"

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

std::set<Data_Area>::iterator DataAreaDeal::GetDataAreaIterator(Data_Area &dataA)
{
    std::set<Data_Area>::iterator it =  m_dataArea.find(dataA);
    if(it == m_dataArea.end())
    {
        //先初始化地址块再插入.
        dataA.index = ( dataA.index/(ADDRESS_AREA_UNIT*8) )*(ADDRESS_AREA_UNIT*8);
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

int32_t DataAreaDeal::GetDataFromAddr(const std::string &reg, uint64_t index, uint64_t len, char *data)
{
    #ifdef DEBUG_LOG
    LOG(INFO)<<"******"<<__func__<<":"<<reg<<index<<" "<<len;
    #endif
    Data_Area dataArea;
    dataArea.reg = reg;
    dataArea.index = index;
    uint64_t offset = 0;
    char * dst = nullptr;

    //开始解析.
    uint32_t freeBitLen =  ((ADDRESS_AREA_UNIT)*8) - (dataArea.index%(ADDRESS_AREA_UNIT*8));
    if( freeBitLen >= len )
    {//在一个地址块中.
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
    {//分散在多个地址块中.
        uint32_t leftByteLen;
        uint32_t totalByte;
        uint32_t rightByteLen;
        uint32_t midUnitLen;
        leftByteLen =  (freeBitLen/8);
        totalByte = static_cast<uint32_t>( len/8 );
        rightByteLen = (totalByte - leftByteLen)%ADDRESS_AREA_UNIT;
        midUnitLen = (totalByte - leftByteLen)/ADDRESS_AREA_UNIT;

        //1.left
        std::set<Data_Area>::iterator it =  GetDataAreaIterator(dataArea);
        if(it == m_dataArea.end())
        {
            return -1;
        }
        offset = dataArea.index - it->index;
        //内存复制.
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
            //内存复制.
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
        //内存复制.
        dst = data+midUnitLen*ADDRESS_AREA_UNIT+leftByteLen;
        memcpy(dst,(*it2).data,rightByteLen);
    }
    return 0;
}

int32_t DataAreaDeal::SetDataFromAddr(const AddrInfo &addr, char *data)
{
    #ifdef DEBUG_LOG
    LOG(INFO)<<"******"<<__func__<<":"<<addr.reg<<addr.index<<" "<<addr.dataLen;
    #endif
    //按字节补齐长度.
    //CompleteByByte(index,len);

    //初始化数据.
    Data_Area dataArea;
    dataArea.reg = addr.reg;
    dataArea.index = addr.index;
    uint64_t offset = 0;
    char * dataA = nullptr;
    char * dst = nullptr;
    char * src = nullptr;
    uint32_t memCpLen = 0;

    //开始解析.
    uint32_t freeBitLen =  ((ADDRESS_AREA_UNIT)*8) - (dataArea.index%(ADDRESS_AREA_UNIT*8));
    if( freeBitLen >= addr.dataLen )
    {//在一个地址块中.
        std::set<Data_Area>::iterator it =  GetDataAreaIterator(dataArea);
        if(it == m_dataArea.end())
        {
            return -1;
        }
        dataA =  const_cast<char *>((*it).data);
        offset = dataArea.index - it->index;
        //开始处理地址变化回调.
        dst = dataA+offset/8;
        src = data;
        memCpLen = static_cast<uint32_t>( addr.dataLen/8 );
        if(0 != Strcmp(dst,src,static_cast<uint32_t>( memCpLen )))
        {
            //地址内存复制.
            memcpy(dst,src,memCpLen);
            //回调.
            //DealAddrValueChangeCB(addr,0);
            m_ValueChangeDeal->DealAddrValueChangeCB(addr,0);
        }
    }
    else
    {//分散在多个地址块中.
        bool bCallbackFlag = false;
        uint32_t leftByteLen =  (freeBitLen/8);
        uint32_t totalByte = static_cast<uint32_t>( addr.dataLen/8 );
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
        //变化回调.
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
            //变化回调.
            dst = dataA;
            src = data +times*ADDRESS_AREA_UNIT+leftByteLen;
            memCpLen = ADDRESS_AREA_UNIT;
            if(0 != Strcmp(dst,src,memCpLen))
            {
                bCallbackFlag = true;
                //内存复制.
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

        //变化回调.
        dst = dataA;
        src = data+midUnitLen*ADDRESS_AREA_UNIT+leftByteLen;
        memCpLen = rightByteLen;
        if(0 != Strcmp(dst,src,memCpLen))
        {
            bCallbackFlag = true;
            //内存复制.
            memcpy(dst,src,memCpLen);
        }

        //4.判断是否变化回调.
        if(bCallbackFlag)
        {
            //DealAddrValueChangeCB(addr,0);
            m_ValueChangeDeal->DealAddrValueChangeCB(addr,0);
        }
    }
    return 0;
}

int32_t DataAreaDeal::SetRegValueChangeCallback(std::vector<Protocol::AddrInfoForRW> &addrVec, Protocol::Add_Type addType)
{
    return m_ValueChangeDeal->SetRegValueChangeCallback(addrVec,addType);
}

