#include "VariableTableDlg.h"
#include "VariableTable.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDir>
#include <QLineEdit>
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

VariableTableDlg::VariableTableDlg(VariableTable *variable, QWidget *parent):QDialog (parent)
  ,m_Variables(variable)
{
    setMinimumSize(800,600);
    setWindowTitle(QString::fromLocal8Bit("数据中心"));
}

void VariableTableDlg::InitUI()
{
    //先读配置
    InitVar();

    //开始初始化界面
    QHBoxLayout * mainLayout = new QHBoxLayout(this);
    QVBoxLayout * leftLayout = new QVBoxLayout();

    //表
    m_varWidget = new QTableWidget(this);
    m_varWidget->setColumnCount(Table_Type_Max);
    QStringList header;
    for (int i = 0;i < Table_Type_Max; ++i)
    {
        header<<Table_Type_Name[i];
    }
    m_varWidget->setHorizontalHeaderLabels(header);
    connect(m_varWidget,SIGNAL(itemDoubleClicked(QTableWidgetItem *)),this,SLOT(doubleClickedSlot(QTableWidgetItem *)));
    connect(m_varWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedSlot(int,int)));


    //类型选择
    QHBoxLayout * leftTopLayout = new QHBoxLayout();
    QLabel * typeLabel = new QLabel(QString::fromLocal8Bit("数据类型选择："),this);
    m_typeCombox = new QComboBox(this);
    leftTopLayout->addWidget(typeLabel);
    leftTopLayout->addWidget(m_typeCombox);
    QStringList typeList = m_typeOfNameMap.keys();
    m_typeCombox->addItems(typeList);
    connect(m_typeCombox,SIGNAL(currentIndexChanged(int)),this,SLOT(typeChangeSlot(int)));
    leftTopLayout->addSpacerItem(new QSpacerItem(1000,1));


    leftLayout->addLayout(leftTopLayout);
    leftLayout->addWidget(m_varWidget);

    QVBoxLayout * butLayout = new QVBoxLayout();
    m_confirm = new QPushButton(QString::fromLocal8Bit("确认"),this);
    connect(m_confirm,SIGNAL(clicked()),this,SLOT(selectComfirmSlot()));
    m_cancle = new QPushButton(QString::fromLocal8Bit("取消"),this);
    connect(m_cancle,SIGNAL(clicked()),this,SLOT(cancleSlot()));
    m_new = new QPushButton(QString::fromLocal8Bit("新增"),this);
    connect(m_new ,SIGNAL(clicked()),this,SLOT(addSlot()));
    m_del = new QPushButton(QString::fromLocal8Bit("删除"),this);
    connect(m_del,SIGNAL(clicked()),this,SLOT(delSlot()));
    m_newType = new QPushButton(QString::fromLocal8Bit("新增类别"),this);
    connect(m_newType ,SIGNAL(clicked()),this,SLOT(addTypeSlot()));
    m_delType = new QPushButton(QString::fromLocal8Bit("删除当前类别"),this);
    connect(m_delType,SIGNAL(clicked()),this,SLOT(delTypeSlot()));
    m_save = new QPushButton(QString::fromLocal8Bit("保存"),this);
    connect(m_save,SIGNAL(clicked()),this,SLOT(saveSlot()));
    butLayout->addWidget(m_confirm);
    butLayout->addWidget(m_cancle);
    butLayout->addWidget(m_new);
    butLayout->addWidget(m_del);
    butLayout->addWidget(m_newType);
    butLayout->addWidget(m_delType);
    butLayout->addWidget(m_save);
    butLayout->setSpacing(20);
    butLayout->addSpacerItem(new QSpacerItem(1,1000));

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(butLayout);

    //切换到第一个类型
    typeChangeSlot(0);
}

void VariableTableDlg::InitVar()
{
    QDir dir("./project/variables");
    QFileInfoList fileList=dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::Hidden|QDir::NoDotAndDotDot,QDir::Name);
    for(int i = 0; i < fileList.size();i++)
    {
        if(fileList.at(i).suffix() == "vars")
        {
            loadVarsFile(fileList.at(i).filePath());
        }
    }
}

void VariableTableDlg::typeChangeSlot(int index)
{
    Q_UNUSED(index);
    //清除
    while(m_varWidget->rowCount() > 0)
    {
        m_varWidget->removeRow(0);
    }
    //添加
    QMap<QString,QStringList>::Iterator it = m_typeOfNameMap.find(m_typeCombox->currentText());
    if(it != m_typeOfNameMap.end())
    {
        m_varWidget->setRowCount(it.value().size());
        int row = 0;
        foreach (auto item, it.value()) {
           initRowFromAddr(row++,item,m_Variables->m_varMap.find(item.toStdString()).value());
        }
    }
}

void VariableTableDlg::selectComfirmSlot()
{
    QList<QTableWidgetItem*> itemList = m_varWidget->selectedItems();
    if(itemList.size() > 0)
    {
        QTableWidgetItem *item = m_varWidget->item(itemList.at(0)->row(),varName);
        if(item)
        {
            m_selectVar = item->text();
        }
        accept();
    }
    else
    {
        m_selectVar = "";
        reject();
    }
}

void VariableTableDlg::cancleSlot()
{
    m_selectVar = "";
    reject();
}

void VariableTableDlg::addSlot()
{
    if(0 == m_typeCombox->count())
    {
        return;
    }
    //增加一行
    int rowcount = m_varWidget->rowCount();
    m_varWidget->setRowCount(rowcount+1);

    Protocol::AddrInfoForRW tempPro;
    tempPro.linkName = "local";
    tempPro.reg = "LW";
    tempPro.index = 0;
    tempPro.dataType = Protocol::bit16;

    //新建名称创建
    QString nameNew;
    QMap<QString,QStringList>::Iterator it =  m_typeOfNameMap.find(m_typeCombox->currentText());
    if(it != m_typeOfNameMap.end())
    {
        while(1)
        {
            nameNew = NEW_ADD_NANE + QString::number(m_Newcout++);
            int index = it.value().indexOf(nameNew);
            if(-1 == index)
            {//不存在
                break;
            }
        }
    }
    else
    {
        return;
    }

    //配置行属性
    initRowFromAddr(rowcount,nameNew,tempPro);

    //容器新增
    it.value().push_back(nameNew);
    m_Variables->m_varMap.insert(nameNew.toLocal8Bit().data(),tempPro);
}

void VariableTableDlg::delSlot()
{
    QList<QTableWidgetItem*> selectItems =  m_varWidget->selectedItems();
    foreach (auto item, selectItems)
    {
        int row = item->row();
        m_varWidget->removeRow(row);
        QMap<QString,QStringList>::Iterator it =  m_typeOfNameMap.find(m_typeCombox->currentText());
        if(it != m_typeOfNameMap.end())
        {
            it.value().removeAll(m_varWidget->item(row,varName)->text());
        }
        m_Variables->m_varMap.remove(m_varWidget->item(row,varName)->text().toLocal8Bit().data());
    }
}

void VariableTableDlg::addTypeSlot()
{
    QDialog tmDlg(this);
    tmDlg.setWindowTitle(QString::fromLocal8Bit("新增类别"));
    QHBoxLayout * mainLayout = new QHBoxLayout();
    QLabel *lable = new QLabel(QString::fromLocal8Bit("名称："),&tmDlg);
    QLineEdit * edit = new QLineEdit(&tmDlg);
    connect(edit,&QLineEdit::returnPressed,[&,this](){
        tmDlg.accept();
        m_typeCombox->addItem(edit->text());
        m_typeOfNameMap.insert(edit->text(),QStringList());
    });
    mainLayout->addWidget(lable);
    mainLayout->addWidget(edit);
    tmDlg.setLayout(mainLayout);
    tmDlg.exec();
}

void VariableTableDlg::delTypeSlot()
{
   QString typeName = m_typeCombox->currentText();
   QMap<QString,QStringList>::Iterator it = m_typeOfNameMap.find(typeName);
   if(it != m_typeOfNameMap.end())
   {
       foreach (auto item, it.value()) {
          m_Variables->m_varMap.remove(item.toLocal8Bit().data());
       };
       m_typeOfNameMap.remove(typeName);
   }
   m_typeCombox->removeItem(m_typeCombox->currentIndex());
}

void VariableTableDlg::saveSlot()
{
    for (QMap<QString,QStringList>::Iterator it = m_typeOfNameMap.begin();it != m_typeOfNameMap.end();++it)
    {
        saveVarsFile(it.key());
    }
}

void VariableTableDlg::dataTypeChangeSlot(int index)
{
    QWidget * combox =qobject_cast<QComboBox*>( sender() );
    if(combox)
    {
        for(int i = 0; i < m_varWidget->rowCount();++i)
        {
            if(m_varWidget->cellWidget(i,dataType) == combox)
            {
                if(index != Protocol::string-1)
                {
                    m_varWidget->item(i,len)->setText("");
                    m_varWidget->item(i,len)->setFlags(m_varWidget->item(i,len)->flags() & (~Qt::ItemIsEditable));
                }
                else
                {
                    m_varWidget->item(i,len)->setFlags(m_varWidget->item(i,len)->flags() | (Qt::ItemIsEditable));
                    m_varWidget->item(i,len)->setText("32");
                }
                //保存数据
                cellChangedSlot(i,dataType);
            }
        }
    }
}

void VariableTableDlg::doubleClickedSlot(QTableWidgetItem *item)
{
    int row = m_varWidget->row(item);
    int col = m_varWidget->column(item);
    if(col == varName)
    {
        QDialog tmDlg(this);
        tmDlg.setWindowTitle(QString::fromLocal8Bit("变量名称修改"));
        QHBoxLayout * mainLayout = new QHBoxLayout();
        QLabel *lable = new QLabel(QString::fromLocal8Bit("名称："),&tmDlg);
        QLineEdit * edit = new QLineEdit(&tmDlg);
        connect(edit,&QLineEdit::returnPressed,[&,this](){
            tmDlg.accept();
            QMap<QString,QStringList>::Iterator it =  m_typeOfNameMap.find(m_typeCombox->currentText());
            if(it != m_typeOfNameMap.end())
            {
                it.value().replace(it.value().indexOf(item->text()),edit->text());
            }
            QMap<std::string,Protocol::AddrInfoForRW >::Iterator itFind = m_Variables->m_varMap.find(
                        m_varWidget->item(row,col)->text().toLocal8Bit().data());
            if(itFind != m_Variables->m_varMap.end())
            {
                m_Variables->m_varMap.insert(edit->text().toLocal8Bit().data(),itFind.value());
                m_Variables->m_varMap.remove(m_varWidget->item(row,col)->text().toLocal8Bit().data());
            }
            m_varWidget->item(row,col)->setText(edit->text());
        });
        mainLayout->addWidget(lable);
        mainLayout->addWidget(edit);
        tmDlg.setLayout(mainLayout);
        tmDlg.exec();
    }
}

void VariableTableDlg::cellChangedSlot(int row, int column)
{
    QMap<std::string,Protocol::AddrInfoForRW >::Iterator itFind = m_Variables->m_varMap.find(
                m_varWidget->item(row,varName)->text().toLocal8Bit().data());
    if(itFind == m_Variables->m_varMap.end())
    {
        return;
    }
    switch (column) {
    case linkName:
    {
        itFind.value().linkName = m_varWidget->item(row,column)->text().toLocal8Bit().data();
    }
        break;
    case reg:
    {
        itFind.value().reg = m_varWidget->item(row,column)->text().toLocal8Bit().data();
    }
        break;
    case index:
    {
        itFind.value().index= m_varWidget->item(row,column)->text().toUInt();
    }
        break;
    case dataType:
    {
        itFind.value().dataType = static_cast<Protocol::Protocol_DataType>( qobject_cast<QComboBox*>(m_varWidget->cellWidget(row,column))->currentIndex()+1 );
    }
        break;
    case len:
    {
        itFind.value().len= m_varWidget->item(row,column)->text().toUInt();
    }
        break;
    }
}

void VariableTableDlg::loadVarsFile(const QString &filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    QStringList nameList;
    QString typeName = filePath.split('/').last().split('.').first();
    QJsonParseError jsonParserError;
    QJsonDocument outDoc = QJsonDocument::fromJson(file.readAll(),&jsonParserError);
    if(!outDoc.isNull() && outDoc.isArray() &&(jsonParserError.error == QJsonParseError::NoError))
    {
        QJsonArray itemArray =  outDoc.array();
        Protocol::AddrInfoForRW addrinfo;
        foreach (auto obj, itemArray)
        {
           QString sVarName =  obj[Table_Type_Name[varName]].toString();
           addrinfo.linkName = obj[Table_Type_Name[linkName]].toString().toStdString();
           addrinfo.reg = obj[Table_Type_Name[reg]].toString().toStdString();
           addrinfo.index = static_cast<uint64_t>( obj[Table_Type_Name[index]].toInt() );
           addrinfo.dataType =static_cast<Protocol::Protocol_DataType>( obj[Table_Type_Name[dataType]].toInt() );
           addrinfo.len = static_cast<uint64_t>( obj[Table_Type_Name[len]].toInt() );
           m_Variables->m_varMap.insert(sVarName.toStdString(),addrinfo);
           nameList.push_back(sVarName);
        }
        m_typeOfNameMap.insert(typeName,nameList);
    }
}

void VariableTableDlg::saveVarsFile(const QString &name)
{
    QString path = "./project/variables/"+name+".vars";
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QString sdata;
    QJsonArray itemsArray;
    QMap<QString,QStringList>::Iterator it = m_typeOfNameMap.find(name);
    if(it != m_typeOfNameMap.end())
    {
        foreach (auto item, it.value()) {
            QMap<std::string,Protocol::AddrInfoForRW >::Iterator it1 = m_Variables->m_varMap.find(item.toStdString());
            if(it1 == m_Variables->m_varMap.end())
            {
                continue;
            }
           Protocol::AddrInfoForRW addr =  it1.value();
           QJsonObject itemObj;
           itemObj[Table_Type_Name[varName]] = item;
           itemObj[Table_Type_Name[linkName]] = QString::fromLocal8Bit( addr.linkName.data() );
           itemObj[Table_Type_Name[reg]] = QString::fromLocal8Bit( addr.reg.data() );
           itemObj[Table_Type_Name[index]] = static_cast<int>( addr.index );
           itemObj[Table_Type_Name[dataType]] = static_cast<int>( addr.dataType );
           itemObj[Table_Type_Name[len]] = static_cast<int>( addr.len );
           itemsArray.append(itemObj);
        }
    }

    //json转字节
    QJsonDocument document;
    document.setArray(itemsArray);
    sdata = document.toJson();

    //写入
    file.write(sdata.toUtf8());
}

void VariableTableDlg::initRowFromAddr(int row, const QString &name, const Protocol::AddrInfoForRW &addr)
{
    //配置行属性
    QTableWidgetItem * tmpItem = new QTableWidgetItem(name);
    tmpItem->setFlags(tmpItem->flags() & (~Qt::ItemIsEditable));
    m_varWidget->setItem(row,varName,tmpItem);

    m_varWidget->setItem(row,linkName,new QTableWidgetItem(QString::fromLocal8Bit( addr.linkName.data() )));
    m_varWidget->setItem(row,reg,new QTableWidgetItem(QString::fromLocal8Bit( addr.reg.data() )));
    m_varWidget->setItem(row,index,new QTableWidgetItem(QString::number(addr.index)));

    QComboBox *tmpbox = new QComboBox(this);
    connect(tmpbox,SIGNAL(currentIndexChanged(int)),this,SLOT(dataTypeChangeSlot(int)));
    tmpbox->addItems(DataType_Names);
    tmpbox->setCurrentIndex( addr.dataType - 1 );
    m_varWidget->setCellWidget(row,dataType,tmpbox);

    m_varWidget->setItem(row,len,new QTableWidgetItem(QString::number(addr.len)));
}
