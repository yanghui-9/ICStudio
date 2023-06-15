#ifndef ADDREDITWIDGET_H
#define ADDREDITWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include "../CommunicationProtocol/common/protocol_type.h"

class AddrEditWidget : public QWidget
{
    Q_OBJECT
public:

    explicit AddrEditWidget(QWidget *parent = nullptr);
    void InitData(const Protocol::AddrInfoForRW &addr,int type = 0);


    QComboBox * m_typeCombox;
    QLineEdit * m_linkenameEdit;
    QLineEdit * m_regEdit;
    QLineEdit * m_indexEdit;
    QComboBox * m_dataTypeCombox;

signals:
    void Changed(const QString &);
};

#endif // ADDREDITWIDGET_H
