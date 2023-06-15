#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "../../communication/communicationdef.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::disconnect(const std::string &ip, int port)
{
   qDebug()<<"client:"<<__func__<<QString::fromStdString(ip)<<port;
}

void MainWindow::connect(const std::string &ip, int port)
{
    qDebug()<<"client:"<<__func__<<QString::fromStdString(ip)<<port;
}

int MainWindow::readReady(int code, char *buf, uint32_t len,char **sBuf, uint32_t *sLen)
{
    qDebug()<<"read:"<<__func__<<QString::fromUtf8(buf,len);
    static std::string sendTest;
    sendTest = "test11111111111111111222222222222222";
    *sBuf = const_cast<char *>(sendTest.data());
    *sLen = sendTest.size();
    return len;
}

//tcp服务端初始化
void MainWindow::on_pushButton_clicked()
{
   icomm = CreateComm(ICommunication::tcp_server,"8888");
   icomm->OpenComm(0,500,std::bind(&MainWindow::connect,this,std::placeholders::_1,std::placeholders::_2),
                   std::bind(&MainWindow::disconnect,this,std::placeholders::_1,std::placeholders::_2),
                   std::bind(&MainWindow::readReady,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3,
                             std::placeholders::_4,std::placeholders::_5));
}

//tcp服务端-事件循环接口
void MainWindow::on_pushButton_2_clicked()
{
    if(icomm)
    {
        icomm->AsynProcess();
    }
}

//tcp客户端初始化
void MainWindow::on_pushButton_3_clicked()
{
  if(!iCommTcpCleint)
  {
      iCommTcpCleint = CreateComm(ICommunication::tcp_client,"127.0.0.1-9999");
      iCommTcpCleint->OpenComm();
  }
}

//tcp客户端发送
void MainWindow::on_pushButton_4_clicked()
{
    if(iCommTcpCleint)
    {
        std::string sendS = "test tcp client send";
        int ret =  iCommTcpCleint->SendData(const_cast<char *>( sendS.data() ),sendS.size());
        qDebug()<<"client send:"<<ret;
    }
}

//tcp客户端接收
void MainWindow::on_pushButton_5_clicked()
{
    if(iCommTcpCleint)
    {
        char rcvBuf[1024] = {0};
        int iLen = 1024;
        int ret =  iCommTcpCleint->RecvData(rcvBuf,iLen,200);
        if(ret > 0)
        {
            qDebug()<<"client rcv:"<<ret<<QString::fromUtf8(rcvBuf,ret);
        }
        else
        {
            qDebug()<<"client rcv:"<<ret;
        }
    }
}

//tcp客户端-发-收
void MainWindow::on_pushButton_6_clicked()
{
    if(iCommTcpCleint)
    {
        std::string sendS = "test tcp client send";
        iCommTcpCleint->AsynWR(const_cast<char *>( sendS.data() ),sendS.size(),
                               std::bind(&MainWindow::RWCB,this,std::placeholders::_1,
                                         std::placeholders::_2,
                                         std::placeholders::_3),1024,500);
    }
}

//tcp客户端异步回调
void MainWindow::RWCB(int code, char *buf, int len)
{
    if(code != -1 )
    {
        qDebug()<<__func__<<code<<QString::fromUtf8(buf,len);
    }
    else
    {
        qDebug()<<__func__<<code;
    }
}
