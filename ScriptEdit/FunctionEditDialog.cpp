#include "FunctionEditDialog.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

FunciontEditDialog::FunciontEditDialog(QWidget *parent,const QString &name, const QString &from) :
    QDialog(parent),m_name(name)
{
   setMinimumSize(600,400);
   QGridLayout * mainLayout = new QGridLayout(this);

   QJsonParseError jsonParserError;
   QJsonDocument outDoc = QJsonDocument::fromJson(from.toUtf8().data(),&jsonParserError);
   if(!outDoc.isNull() && (jsonParserError.error == QJsonParseError::NoError))
   {
       int row = 0;
       if(outDoc.isArray())
       {//item array
           QJsonArray itemArray =  outDoc.array();
           QLabel * label = nullptr;
           QLineEdit * lineEdit = nullptr;
           QComboBox * comBox = nullptr;
           foreach (auto link, itemArray)
           {//item
               QJsonObject itemobj =  link.toObject();
                if(0 == row)
                {//note
                    QTextEdit * edit = new QTextEdit(this);
                    edit->setText(itemobj["note"].toString());
                    mainLayout->addWidget(edit,row,0,1,2);
                    row++;
                    continue;
                }
                itemobj["name"].toString();
                itemobj["type"].toInt();
                itemobj["args"].toString();
                itemobj["index"].toInt();
                label = new QLabel(itemobj["name"].toString(),this);
                mainLayout->addWidget(label,row,0);
                if(0 == itemobj["type"].toInt())
                {//编辑框
                    lineEdit = new QLineEdit(itemobj["args"].toString(),this);
                    mainLayout->addWidget(lineEdit,row,1);
                    m_items.push_back(Item_Pakeage(lineEdit,0));
                }
                else
                {//下拉框
                   comBox = new QComboBox(this);
                   comBox->addItems(itemobj["args"].toString().split('#'));
                   comBox->setCurrentIndex(itemobj["index"].toInt());
                   mainLayout->addWidget(comBox,row,1);
                   m_items.push_back(Item_Pakeage(comBox,1));
                }
                row++;
           }
       }
       //确认&退出
       QHBoxLayout *  btnlayout =  new QHBoxLayout(this);
       QPushButton * okBtn = new QPushButton("ok",this);
       QPushButton * cancleBtn = new QPushButton("cancle",this);
       connect(okBtn,SIGNAL(clicked()),this,SLOT(okSlot()));
       connect(cancleBtn,SIGNAL(clicked()),this,SLOT(cancleSlot()));
       btnlayout->addWidget(okBtn);
       btnlayout->addWidget(cancleBtn);
       mainLayout->addLayout(btnlayout,row,1);
   }
   setLayout(mainLayout);
}

FunciontEditDialog::~FunciontEditDialog()
{
}

QString FunciontEditDialog::GetRusult()
{
    m_name +="(";
    foreach (auto item, m_items)
    {
        if(0 == item.type)
        {
            m_name += ((QLineEdit*)item.item)->text();
        }
        else
        {
            m_name += ((QComboBox*)item.item)->currentText();
        }
        m_name +=",";
    }
    if(m_items.size() == 0)
    {
        m_name += ")";
    }
    else
    {
        m_name.back() =')';
    }
    return m_name;
}

void FunciontEditDialog::okSlot()
{
    QDialog::accept();
}

void FunciontEditDialog::cancleSlot()
{
    QDialog::reject();
}
