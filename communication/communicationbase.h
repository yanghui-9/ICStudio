#ifndef COMMUNICATIONBASE_H_H
#define COMMUNICATIONBASE_H_H
#include "communicationdef.h"
#include "io/inc/io_log_service.h"
#include "io/inc/io_util.h"

class communicationbase: public ICommunication
{
public:
    communicationbase();
    virtual ~communicationbase();
    int m_logFlag;
    long m_lRef;
    io_lock_t m_logLock;
    int LOG_DBG2(io_log_service_t *logObj, int q_hour, int q_min, int q_sec, int q_msec, const char *buf1, int len1, const char *fmt,...);
    void win_unix_sleep(unsigned long sec);

    virtual int32_t OpenLog(const char * LogName,int AscOrDec =0,int OpenType = 2);
    virtual int32_t CloseLog();
    virtual int32_t WriteLog(const char *log);

    virtual int32_t Reset(const char * szIP = NULL,  uint32_t iPort = 0){ return -1;}
    virtual int32_t ServerDispatch(){ return -1;}

    virtual int32_t OpenComm(int setnonblock,int Timo = 500){ return -1;}
    virtual int32_t CloseComm(){ return -1;}
    virtual int32_t SetCommInfo(std::string sPort, STOPBIT eStop, PARITYBIT eParitBit,
                                BAUDRATE eBaud, DATABIT eDataBit,int serialPollingTime=10,FlOWCTR fc = FLOW_NONE){ return -1;}
    virtual int32_t GetCommInfo(std::string sPort, STOPBIT * pStop, PARITYBIT * pParitBit,
                                BAUDRATE * pBaud, DATABIT * pDataBit,int * serialPollingTime,FlOWCTR *fc){ return -1;}
    virtual int32_t SendData(const char *pBuffer, int32_t iLength, uint32_t idForServerToClientOrUdpPort = 0, const char *UdpIp=NULL){ return -1;}
    virtual int32_t RecvData(char * pBuffer, int32_t iLength, uint32_t uiDelayTime,
                                         uint32_t idForServerToClientOrUdpPort = 0, RECV_TYPE recvType = RECV_TYPE_ALL,  char *szStopOrUdpIp = NULL){ return -1;}
    virtual int32_t GetCommInfo(std::vector<clientPro>*proList) {return -1;}
    virtual int32_t SetCommCallBackFunc(void *on_connect, void *on_processor, void *on_error, void *on_destory){return -1;}


    void _emit(int len)
    {
        foreach (auto mslot, m_connected_slots)
        {
              mslot->_emit(len);
        }
    }
    io_log_service_t *m_io_log_service;
};

#endif // COMMUNICATIONBASE_H
