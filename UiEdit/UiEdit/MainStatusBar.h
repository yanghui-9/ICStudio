#ifndef MAINSTATUSBAR_H
#define MAINSTATUSBAR_H
#include    <QStatusBar>
#include    <QLabel>
#include    <QLineEdit>

class MainStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit MainStatusBar(QWidget *parent = nullptr);

public:
    void retranslateUI(void);
    void refreshUIfromData();

public:
    QLabel * nameLabel;
    QLabel * posStatus;
    QLabel * itemLeft;
    QLabel * itemTop;
    QLabel * itemSize;
    QLabel * itemWidth;
    QLabel * itemHeight;
	QLabel * name;
    QLineEdit * leftEdit;
    QLineEdit * topEdit;
    QLineEdit * widthEdit;
    QLineEdit * heightEdit;
};

#endif // MAINSTATUSBAR_H
