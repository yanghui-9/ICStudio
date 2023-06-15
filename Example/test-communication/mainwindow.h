#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class ICommunication;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //Tcp服务端对应事件的回调函数，分别是断开连接，新连接，有数据
    void disconnect(const std::string &ip,int port);
    void connect(const std::string &ip,int port);
    int readReady(int code, char *buf, uint32_t len, char **sBuf, uint32_t *sLen);

private slots:

    //tcp服务端初始化
    void on_pushButton_clicked();
    //tcp服务端-事件循环接口
    void on_pushButton_2_clicked();
    //tcp客户端初始化
    void on_pushButton_3_clicked();
    //tcp客户端发送
    void on_pushButton_4_clicked();
    //tcp客户端接收
    void on_pushButton_5_clicked();
    //tcp客户端-发-收
    void on_pushButton_6_clicked();

public:
    //tcp客户端异步回调
    void RWCB(int code, char *buf,int len);

private:
    Ui::MainWindow *ui;
    //tcp服务端对象
    ICommunication * icomm = nullptr;
    //tcp客户端对象
    ICommunication * iCommTcpCleint = nullptr;
};

#endif // MAINWINDOW_H
