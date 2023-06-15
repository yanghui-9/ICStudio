#include "LabelBase.h"
#include <QGridLayout>
#include <QLabel>
#include <QTextEdit>

LabelBase::LabelBase(QWidget *parent) : QGroupBox (parent)
{
    //背景设置
    setTitle(QString::fromLocal8Bit("标签设置"));

    QGridLayout * bgLayout = new QGridLayout();
    QLabel * label = new QLabel(QString::fromLocal8Bit("文本："),this);
    textEdit = new QTextEdit(this);
    bgLayout->addWidget(label,0,0,1,1);
    bgLayout->addWidget(textEdit,0,1,1,1);

    setLayout(bgLayout);
}
