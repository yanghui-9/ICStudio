#include "protocol_interface.h"
#include <QStringList>
#include "typedefheader.h"

using namespace Protocol;

void Protocol_Interface::GetGlobalFrame()
{
}

int32_t Protocol_Interface::AddrS2D(const std::string &reg, const std::string &sIndex, uint64_t &index)
{
    bool ok;
    ok = false;
    bool ok1;
    ok1 = false;
    QString sIn = QString::fromStdString(sIndex);
    Device &D = Device::GetInstance();

    std::map<std::string,Protocol::RegInfo>::iterator it = D.m_regInfoMap.find(reg);
    if(it != D.m_regInfoMap.end())
    {
        //进制处理.
        char Dbase;
        Dbase = 'D';
        if( 0 < it->second.format.size() )
        {
            Dbase = it->second.format.at(0);
        }
        else
        {
            return -1;
        }
        switch (Dbase) {
        case 'D':
        {
            Dbase = 10;
        }
            break;
        case 'O':
        {
            Dbase = 8;
        }
            break;
        case 'H':
        {
            Dbase = 16;
        }
            break;
        }

        //开始转换.
        ok = true;
        ok1 = true;
        if(-1 != sIn.indexOf('.'))
        {//包含小数点.
            QStringList sList =  sIn.split('.');
            if(2 == sList.size())
            {
                index = sList.at(0).toULong(&ok,Dbase)*(it->second.unitBitNum) + sList.at(1).toULong(&ok1,Dbase);
            }
        }
        else
        {
           index = sIn.toULong(&ok,Dbase);
        }
    }

    if(ok && ok1)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int32_t Protocol_Interface::AddrD2S(const std::string &reg, const uint64_t &index, std::string &sIndex)
{
    QString sFormat;
    Device &D = Device::GetInstance();

    std::map<std::string,Protocol::RegInfo>::iterator it = D.m_regInfoMap.find(reg);
    if(it != D.m_regInfoMap.end())
    {
        //进制处理.
        char Dbase;
        Dbase = 'D';
        if( 0 < it->second.format.size() )
        {
            Dbase = it->second.format.at(0);
            sFormat = QString::fromStdString(it->second.format);
        }
        else
        {
            return -1;
        }
        switch (Dbase) {
        case 'D':
        {
            Dbase = 10;
        }
            break;
        case 'O':
        {
            Dbase = 8;
        }
            break;
        case 'H':
        {
            Dbase = 16;
        }
            break;
        }

      //开始转换.
      if(-1 != sFormat.indexOf('.'))
      {
          sIndex = QString("%1.%2").arg(index/it->second.unitBitNum,0,Dbase,QChar(' '))
                  .arg(index%it->second.unitBitNum,0,Dbase,QChar(' ')).toStdString();
      }
      else
      {
          sIndex = QString::number(index,static_cast<int32_t>( Dbase )).toStdString();
      }
      return 0;
    }
    else
    {
        return -1;
    }
}

void Protocol_Interface::DealByteOrder(char *value, const uint32_t &dataType)
{
    char value1[4] = {0};

    //处理字节序.
    switch (dataType) {
    case Protocol::bit:
    case Protocol::bit8:
    case Protocol::ubit8:
    {
        //
    }
        break;
    case Protocol::ubit16:
    case Protocol::bit16:
    {
        //处理字节序.
        if(BA == Device::GetInstance().m_deviceConfig.protocolInfo.byteOrder16)
        {
            value1[1] = value[1];
            value[1] = value[0];
            value[0] = value1[1];
        }
    }
        break;
    case Protocol::ubit32:
    case Protocol::bit32:
    case Protocol::float32:
    {
        switch (Device::GetInstance().m_deviceConfig.protocolInfo.byteOrder32) {
        case BADC:
        {
            value1[1] = value[1];
            value[1] = value[0];
            value[0] = value1[1];

            value1[3] = value[3];
            value[3] = value[2];
            value[2] = value1[3];
        }
            break;
        case CDAB:
        {
            value1[2] = value[2];
            value[2] = value[0];
            value[0] = value1[2];

            value1[3] = value[3];
            value[3] = value[1];
            value[1] = value1[3];
        }
            break;
        case DCBA:
        {
            value1[3] = value[3];
            value[3] = value[0];
            value[0] = value1[3];

            value1[2] = value[2];
            value[2] = value[1];
            value[1] = value1[2];
        }
            break;
        }
    }
        break;
    }
}

int32_t Protocol_Interface::DealRecvData(AddrInfo &addrinfo, char *data)
{
    //asc转换hex处理.
    if(Device::frame_asc == Device::GetInstance().m_iFrameDataFormat)
    {
        data = data + addrinfo.beginIndex;
        uint64_t sizeB;
        sizeB = addrinfo.len/8;
        for (uint32_t i = 0 ; i < sizeB/2; i++)
        {
            data[i] = static_cast<char>( (a2d_(*(data+(i*2+0))) << 4) + a2d_(*(data+(i*2+1))) );
        }
        addrinfo.len = addrinfo.len/2;
    }
    return 0;
}

void Protocol_Interface::DealDataOfSlave(char *data, uint32_t len, char **sendBuff, uint32_t *slen)
{
    Q_UNUSED(data);
    Q_UNUSED(len);
    Q_UNUSED(sendBuff);
    Q_UNUSED(slen);
}

bool Protocol_Interface::JudgeAddrBlockMerge(AddrInfo &addr0, AddrInfo &addr1, int32_t intervallen, int32_t maxLen)
{//默认排序过进来的.
    if(addr0.reg == addr1.reg
            && (static_cast<int64_t>(addr1.index) - static_cast<int64_t>(addr0.index+addr0.len) ) <= intervallen)
    {
        uint64_t newLen = ( (addr0.index+addr0.len >= addr1.index+addr1.len)?addr0.index+addr0.len:addr1.index+addr1.len ) - addr0.index;
        if(newLen > static_cast<uint32_t>( maxLen ))
        {//如果合并之后的长度已经大于最大长度.
            return false;
        }
        addr0.len = static_cast<uint32_t>( newLen );
        //对应的变量也插入变量列表.
        addr0.varList.insert(addr0.varList.end(),addr1.varList.begin(),addr1.varList.end());
        return true;
    }
    else
    {
        return false;
    }
}
