#include "MainStatusBar.h"
#include "QIntValidator"
#include "QLineEdit"

MainStatusBar::MainStatusBar(QWidget *parent) :
    QStatusBar(parent)
{
    //style
    this->setStyleSheet("background-color:rgb(165,165,165);");
    this->setSizeGripEnabled(false); //去掉最右边的大小控制点竖条
    //label name
    nameLabel = new QLabel(this);
    posStatus = new QLabel(this);
    itemLeft = new QLabel(this);
    leftEdit = new QLineEdit(this);
    itemTop = new QLabel(this);
    topEdit = new QLineEdit(this);
    itemSize = new QLabel(this);
    itemWidth = new QLabel(this);
    widthEdit = new QLineEdit(this);
    itemHeight = new QLabel(this);
    heightEdit = new QLineEdit(this);

    //TODO:设置样式
    leftEdit->setStyleSheet("QLineEdit:hover{ background:rgb(255,85,0); }"
                        "QLineEdit:focus {background:rgb(255,255,255); }");
    topEdit->setStyleSheet("QLineEdit:hover{ background:rgb(255,85,0); }"
                        "QLineEdit:focus {background:rgb(255,255,255); }");
    widthEdit->setStyleSheet("QLineEdit:hover{ background:rgb(255,85,0); }"
                        "QLineEdit:focus {background:rgb(255,255,255); }");
    heightEdit->setStyleSheet("QLineEdit:hover{ background:rgb(255,85,0); }"
                        "QLineEdit:focus {background:rgb(255,255,255); }");

    name = new QLabel(this);

    refreshUIfromData();
}

void MainStatusBar::retranslateUI(void)
{
    nameLabel->setText("");

    posStatus->setText(QString::fromLocal8Bit("位置:"));

    itemLeft->setText("X:");

    leftEdit->setObjectName("statusbar:left");

    itemTop->setText("Y:");

    topEdit->setObjectName("statusbar:top");

    itemSize->setText(QString::fromLocal8Bit("大小:"));

    itemWidth->setText("W:");

    widthEdit->setObjectName("statusbar:width");

    itemHeight->setText("H:");

    heightEdit->setObjectName("statusbar:height");
}

void MainStatusBar::refreshUIfromData()
{
    this->insertPermanentWidget(0, nameLabel);

    this->insertPermanentWidget(1, posStatus);

    this->insertPermanentWidget(2, itemLeft);

    leftEdit->setFixedWidth(50);
    this->insertPermanentWidget(3, leftEdit);

    this->insertPermanentWidget(4, itemTop);

    topEdit->setFixedWidth(50);
    this->insertPermanentWidget(5, topEdit);

    this->insertPermanentWidget(6, itemSize);

    this->insertPermanentWidget(7, itemWidth);

    widthEdit->setFixedWidth(50);
    this->insertPermanentWidget(8, widthEdit);

    this->insertPermanentWidget(9, itemHeight);

    heightEdit->setFixedWidth(50);
    this->insertPermanentWidget(10, heightEdit);
    name->setFixedWidth(330);
    this->insertPermanentWidget(11, name);
}
