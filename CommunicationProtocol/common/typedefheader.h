#ifndef TYPEDEFHEADER_H
#define TYPEDEFHEADER_H
#include <stdint.h>
#include <string>
#include <vector>


//改良strcmp，加入长度限制.
static int Strcmp(const char* str1, const char* str2, uint32_t size)
{
    int ret = 0;
    while( size-- && !(ret=*(unsigned char*)str1-*(unsigned char*)str2) )
    {
        str1++;
        str2++;
    }
    if (ret < 0)
    {
        return -1;
    }
    else if (ret > 0)
    {
        return 1;
    }
    return 0;
}

//获取字的位.
inline char GetBitOfByte(char *byte, uint32_t num)
{
    if(((*byte)>>num)&0x1)
    {
        return 1;
    }
    else {
        return 0;
    }
}

//设置字的位.
inline void SetBitOfByte(char *byte, uint32_t num, bool value)
{
    if(!value)
    {
        *byte =  (~(1<<num))&(*byte);
    }
    else
    {
        *byte =  (1<<num)|(*byte);
    }
}
//crc16校验.

static unsigned short cal_crc_(unsigned char *data, unsigned short len)
{
    unsigned short crc_ = 0xffff;
    int i,j;

    for (i=0; i<len; i++)
    {
        crc_ ^= data[i];
        for (j=0; j<8; j++)
        {
            if (crc_ & 1)
            {
                crc_ >>= 1;
                crc_ ^= 0xA001;
            }
            else
            {
                crc_ >>= 1;
            }
        }
    }

    return crc_;
}
//bcc校验.
static unsigned short cal_bcc_(unsigned char *data, unsigned short len)
{
    int i;
    int bcc_ = 0;

    for(i=0; i<len; i++)
    {
        bcc_ = (bcc_ ^ data[i]);
        bcc_ = (bcc_ << 1);
        bcc_ = bcc_ + ((bcc_ >> 8) & 0x01);
    }
    return (bcc_ & 0xff);
}
//和校验sum.
static unsigned short cal_sum_(unsigned char *data, unsigned short len)
{
    int i;
    int sum_ = 0;

    for(i=0; i<len; i++)
    {
        sum_ = sum_ + data[i];
    }

    return (sum_ & 0xff);
}
//LRC校验.
static unsigned short cal_lrc_(unsigned char *data, unsigned short len)
{
    int i;
    int lrc_ = 0;

    for(i=0; i<len; i++)
    {
        lrc_ = lrc_ + data[i];
    }
    lrc_ = ~(lrc_);
    lrc_ = (lrc_ & 0xff) + 1;

    return (lrc_ & 0xff);
}
//XOR校验.
static unsigned short cal_xor_(unsigned char *data, unsigned short len)
{
    int i;
    int xor_;

    xor_ = data[0];
    for(i=1; i<len; i++)
    {
        xor_ = xor_ ^ data[i];
    }

    return (xor_& 0xff);
}


//函数功能： 数字转换为ASC码函数.
static int d2a_(int data)
{
    int change_data = 0;

    if (data < 10)
    {
        change_data = data + 0x30;
    }
    else
    {
        change_data = (data - 10) + 0x41;
    }

    return (change_data);
}


//函数功能： ASC码转换为数字函数.
static int a2d_(int data)
{
    static int change_data;
    change_data = 0;

    if (data >= 0x30)
    {
        if (data < 0x40)
        {
            change_data = data - 0x30;
        }
        else if (data > 0x40)
        {
            change_data = (data - 0x41) + 10;
        }
    }

    return (change_data);
}


//字符串分割函数.
static std::vector<std::string> StringSplit(const std::string &str,const std::string &pattern)
{
    std::vector<std::string> resVec;
    if ("" == str)
    {
        return resVec;
    }
    //方便截取最后一段数据.
    std::string strs = str + pattern;

    size_t pos = strs.find(pattern);
    size_t size = strs.size();
    while (pos != std::string::npos)
    {
        std::string x = strs.substr(0,pos);
        resVec.push_back(x);
        strs = strs.substr(pos+1,size);
        pos = strs.find(pattern);
    }
    return resVec;
}

#endif // TYPEDEFHEADER_H
