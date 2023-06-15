#include "AddrEditWidget.h"
#include <QGridLayout>
#include <QGroupBox>

AddrEditWidget::AddrEditWidget(QWidget *parent) : QWidget(parent)
{
    QGridLayout * layout = new QGridLayout(this);
    QLabel * label = new QLabel("type:",this);
    m_typeCombox = new QComboBox(this);
    layout->addWidget(label,0,0);
    layout->addWidget(m_typeCombox,0,1);
    m_typeCombox->addItem("address");
    m_typeCombox->addItem("tag");
    connect(m_typeCombox,SIGNAL(currentTextChanged(const QString &)),this,SIGNAL(Changed(const QString &)));

    label = new QLabel("linkname:",this);
    m_linkenameEdit = new QLineEdit(this);
    layout->addWidget(label,1,0);
    layout->addWidget(m_linkenameEdit,1,1);
    connect(m_linkenameEdit,SIGNAL(textChanged(const QString &)),this,SIGNAL(Changed(const QString &)));

    label = new QLabel("reg:",this);
    m_regEdit = new QLineEdit(this);
    layout->addWidget(label,2,0);
    layout->addWidget(m_regEdit,2,1);
    connect(m_regEdit,SIGNAL(textChanged(const QString &)),this,SIGNAL(Changed(const QString &)));

    label = new QLabel("index:",this);
    m_indexEdit = new QLineEdit(this);
    layout->addWidget(label,3,0);
    layout->addWidget(m_indexEdit,3,1);
    connect(m_indexEdit,SIGNAL(textChanged(const QString &)),this,SIGNAL(Changed(const QString &)));

    label = new QLabel("datatype:",this);
    m_dataTypeCombox = new QComboBox(this);
    layout->addWidget(label,4,0);
    layout->addWidget(m_dataTypeCombox,4,1);
    m_dataTypeCombox->addItem("bit");
    m_dataTypeCombox->addItem("bit8");
    m_dataTypeCombox->addItem("ubit8");
    m_dataTypeCombox->addItem("ubit16");
    m_dataTypeCombox->addItem("bit16");
    connect(m_dataTypeCombox,SIGNAL(currentTextChanged(const QString &)),this,SIGNAL(Changed(const QString &)));

    this->setLayout(layout);
}

void AddrEditWidget::InitData(const Protocol::AddrInfoForRW &addr, int type)
{
    m_typeCombox->setCurrentIndex(type);
    m_linkenameEdit->setText(QString::fromLocal8Bit( addr.linkName.data() ));
    m_regEdit->setText(QString::fromLocal8Bit(addr.reg.data()));
    m_indexEdit->setText(QString::fromStdString( addr.sIndex ));
    m_dataTypeCombox->setCurrentIndex(addr.dataType);
}
