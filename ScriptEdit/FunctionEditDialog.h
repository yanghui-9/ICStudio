#ifndef FUNCIONTEDITDIALOG_H
#define FUNCIONTEDITDIALOG_H

#include <QDialog>

struct Item_Pakeage
{
    QWidget * item = nullptr;
    int type = 0;
    Item_Pakeage(QWidget *_item,int _type):item(_item),type(_type){}
};

class FunciontEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FunciontEditDialog(QWidget *parent, const QString &name, const QString &from);
    ~FunciontEditDialog();
    QString GetRusult();

public slots:
    void okSlot();
    void cancleSlot();

private:
    QVector<Item_Pakeage> m_items;
    QString m_name;
};

#endif // FUNCIONTEDITDIALOG_H
