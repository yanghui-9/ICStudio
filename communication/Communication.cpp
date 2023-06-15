#include "communicationdef.h"
#include "SerialComm.h"
#include "TcpClient.h"
#include "TcpServer.h"

ICommunication * CreateComm(ICommunication::Communication_Type type, const std::string &sPar)
{
    ICommunication * icomm = nullptr;
    switch (type) {
    case ICommunication::tcp_client:
    {
        icomm = new TcpClient(sPar);
    }
        break;
    case ICommunication::tcp_server:
    {
        icomm = new TcpServer(sPar);
    }
        break;
    case ICommunication::upd:
    {
        //
    }
        break;
    case ICommunication::serial:
    {
         icomm = new SerialComm(sPar);
    }
        break;
    }
    return icomm;
}

int CloseCommunication(ICommunication *pCommunication)
{
    if(pCommunication)
    {
        delete pCommunication;
        pCommunication = nullptr;
        return 0;
    }
    else
    {
        return -1;
    }
}

