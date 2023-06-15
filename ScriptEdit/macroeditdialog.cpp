#include "macroeditdialog.h"
#include "ui_macroeditdialog.h"
#include <QDebug>
#include <QPainter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QDebug>
#include <QToolTip>
#include <QDesktopWidget>
#include "../../lib/lua/src/lua.hpp"
#include "macroconfig.h"
#include "../../basics_model/device_system/addressinput.h"
#include "../../base_include/UAppBase.h"
#include "../../basics_model/device_tools/include/tools.h"
#include "../../basics_model/device_struct/include/cobject.h"



int HMI_UNIMAT_ISEXPRESSION = 0;//1-bool,2-number,0-其他
int l_HMIgetisExpression(lua_State *L)
{
    QString type(lua_tostring(L,1));
    if(type == "boolean")
        HMI_UNIMAT_ISEXPRESSION =1;
    else if(type == "number")
       HMI_UNIMAT_ISEXPRESSION = 2;
    else
       HMI_UNIMAT_ISEXPRESSION = 0;
    return 0;
}
int l_HMIgetDataOffline(lua_State *L)
{
    int data =0;
     //g_sysModule->getMacroInterface()->getCommValueNumber(strlink,reg,index,station,len,data);
     lua_pushnumber(L,(lua_Number)data);
     return 1;
}

macroEditDialog::macroEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::macroEditDialog)
{
    m_macroE = NULL;
    ui->setupUi(this);
    setWindowTitle(QObject::tr("macro"));
    initWidget();
    m_idCount = 0;
    m_copyFlag = -1;
    m_copyId = -1;
}

macroEditDialog::~macroEditDialog()
{
    delete ui;
}

void macroEditDialog::initWidget()
{
     ui->new_->setText(QObject::tr("new"));
     ui->delecte_->setText(QObject::tr("delecte"));
     ui->copy_->setText(QObject::tr("copy"));
     ui->paste_->setText(QObject::tr("paste"));
     ui->edit_->setText(QObject::tr("edit"));
     ui->export_->setText(QObject::tr("exprot"));
     ui->import_->setText(QObject::tr("import"));
     ui->exit_->setText(QObject::tr("exit"));
     ui->save_->setText(QObject::tr("save"));
     ui->varTable_->setText(QObject::tr("var list"));
     ui->example->setText(QObject::tr("example"));
     ui->help_->setText(QObject::tr("Help"));
     ui->groupBox->setTitle(QObject::tr("Compile successfully"));
     ui->groupBox_2->setTitle(QObject::tr("Compilation fails"));

     connect(ui->listWidget,SIGNAL(clicked(QModelIndex)),this,SLOT(deleteItem(QModelIndex)));
     connect(ui->listWidget_2,SIGNAL(clicked(QModelIndex)),this,SLOT(deleteItem(QModelIndex)));

     struct object * root = ((UAppBase *)qApp)->getCurPjt();
     //m_prj = pApp->getCurrentProject();
     //if (m_prj)
     {
         m_macroObj = object_get_child(root, "macro");
         if (!m_macroObj)
         {
            m_macroObj  = object_create("macro");
             object_add_child(root, m_macroObj);
         }
         /*test,yanghui,20200413,begin*/
         m_macroVarListObj = object_get_child(root, "macroVarList");
         if (!m_macroVarListObj)
         {
             m_macroVarListObj  = object_create("macroVarList");
             object_add_child(root, m_macroVarListObj);
             object_create_property_number(m_macroVarListObj,"number",0);
         }
         /*test,yanghui,20200413,end*/
         struct object *links = object_get_child(root,"link");
         if(!links)
             return;
         struct object *child = object_first_child(links);
         while (child)
         {
             QString linkname = object_get_property_value_type_string(child,"linkName");
             linkname =linkname+"#"+QString::number(object_get_property_value_type_int32(child,"linkID"));
             QString blocks  = object_get_property_value_type_string(child,"blockNames");
             m_links.insert(linkname,blocks);
             child = object_next(child);
         }
     }
}

void macroEditDialog::refreshUI()
{
    struct object * macroObj = object_first_child(m_macroObj);
    while (macroObj)
    {
        int flag = object_get_property_value_type_int32(macroObj,"flag");
        QString text = object_get_property_value_type_string(macroObj,"text");
        int id = object_get_property_value_type_int32(macroObj,"id");
        int type= object_get_property_value_type_int32(macroObj,"type");
        QString name = object_get_property_value_type_string(macroObj,"name");
        QString triggerText = object_get_property_value_type_string(macroObj,"triggerText");
        QString varlist = object_get_property_value_type_string(macroObj,"varlist");
        int cycleTime = object_get_property_value_type_int32(macroObj,"cycleTime");
        QString fontType = object_get_property_value_type_string(macroObj,"fontType");
        int fontSize = object_get_property_value_type_int32(macroObj,"fontSize");
        int triggerType = object_get_property_value_type_int32(macroObj,"triggerType");
        int saveBitAddressFlag = object_get_property_value_type_int32(macroObj,"saveBitAddressFlag");
        struct object *  m_saveAddressObj  = createAddressObject(QString("saveBitAddress"),0);
        copyAddressByObj(object_get_child(macroObj,"saveBitAddress"), m_saveAddressObj);
        /*Bug 4049 - 通过脚本配置下添加脚本处理，编译保存，然后再进入里面添加的脚步内容字体变小_yangjindong_20190715_begin*/
        if(fontType=="")
            fontType = "仿宋";
        if(fontSize>72||fontSize<5)
            fontSize = 6;
        /*Bug 4049 - 通过脚本配置下添加脚本处理，编译保存，然后再进入里面添加的脚步内容字体变小_yangjindong_20190715_end*/
        /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,begin*/
        if(0 == flag)
        {
            m_failMacro.append(text);
            m_failMacroPro.append(QString("%1#%2#").arg(id).arg(type) + name+"#"+triggerText+"#"+varlist+
                                  "#"+QString::number(cycleTime)+
                                  "#"+fontType+
                                  "#"+QString::number(fontSize)+
                                  "#"+QString::number(triggerType)+
                                  "#"+QString::number(saveBitAddressFlag));
            m_failsaveBitAddressList<<m_saveAddressObj;
        }
        else
        {
            m_idList<<id;
            /*问题修改，全局脚本数量过多时新增脚本保存有些情况会丢失脚本，yanghui，20180929，begin*/
            if(m_idCount < id +1)
                m_idCount = id +1;
            /*问题修改，全局脚本数量过多时新增脚本保存有些情况会丢失脚本，yanghui，20180929，end*/
            m_sucMacro.append(text);
            if(name.indexOf('#') != -1)
                name.remove('#');
            m_sucMacroPro.append(QString("%1#%2#").arg(id).arg(type) + name+"#"+triggerText+"#"+varlist+
                                 "#"+QString::number(cycleTime)+
                                 "#"+fontType+
                                 "#"+QString::number(fontSize)+
                                 "#"+QString::number(triggerType)+
                                 "#"+QString::number(saveBitAddressFlag));
            m_sucsaveBitAddressList<<m_saveAddressObj;
        }
        macroObj = object_next(macroObj);
    }
    for(int i = 0;i<m_sucMacroPro.size();i++)
    {
        ui->listWidget->addItem(m_sucMacroPro.at(i).split('#').at(2));
    }
    for(int i =0;i<m_failMacroPro.size();i++)
    {
        ui->listWidget_2->addItem(m_failMacroPro.at(i).split('#').at(2));
    }

    if(m_macroVarListObj)
    {
        int number = object_get_property_value_type_int32(m_macroVarListObj,"number");
        for(int i = 0; i < number; i++)
        {
            QString tmpStr =  object_get_property_value_type_string(m_macroVarListObj,QString("var%1").arg(i).toLocal8Bit().data());
            m_macroVarListPro.append(tmpStr);
        }
    }
}

//新增
void macroEditDialog::on_new__clicked()
{
    if(!m_macroE)
    {
        m_macroE =new macroDoubleclickedEditDialog((void *)(&m_links),this);
        m_macroE->initWidget();
    }
    struct object *  Addressobj  = createAddressObject(QString("saveBitAddress"),0);
    m_macroE->setVarList(m_macroVarListPro);
    /*问题修改,db特殊地址作为触发地址时保存不过,yanghui,20200420,begin*/
    m_macroE->refreshUI("",QString("%1_3_macro%2___1000_Arial_8_1_0").arg(m_idCount).arg(m_idCount),0,Addressobj);
    /*问题修改,db特殊地址作为触发地址时保存不过,yanghui,20200420,end*/
    if(m_macroE->exec() == QDialog::Accepted)
    {
        //id检查
        int id = m_macroE->m_idedit->text().toInt();
        if(m_idList.indexOf(id) != -1)
        {
            QMessageBox::warning(this,
                                  QObject::tr( "Warning" ),
                                  QObject::tr( "This ID has already been used!" ),
                                  QObject::tr("Ok") );
            m_macroE->m_flag = 1;
        }
       if(!m_macroE->m_flag)
       {
           m_sucMacro.append(m_macroE->m_text);
           m_sucMacroPro.append(m_macroE->m_textPro);
           m_sucsaveBitAddressList.append(m_macroE->m_saveBitAddressObj);
           ui->listWidget->addItem(m_macroE->m_nameedit->text());
           m_idList<<m_idCount;
           m_idCount++;
       }
       else
       {
           m_failMacro.append(m_macroE->m_text);
           m_failMacroPro.append(m_macroE->m_textPro);
           m_failsaveBitAddressList.append(m_macroE->m_saveBitAddressObj);
           ui->listWidget_2->addItem(m_macroE->m_nameedit->text());
       }
    }
}

//编辑选中脚本
void macroEditDialog::on_edit__clicked()
{
    int cur = ui->listWidget->currentRow();
    /*问题修改，没有脚本时点击编辑按钮报错问题，yanghui，20180702，begin*/
    int cur2 =ui->listWidget_2->currentRow();
    if(cur != -1)
    {
        on_listWidget_doubleClicked(ui->listWidget->currentIndex());
    }
    else if(cur2 != -1)
    {
        on_listWidget_2_doubleClicked(ui->listWidget_2->currentIndex());
    }
    /*问题修改，没有脚本时点击编辑按钮报错问题，yanghui，20180702，end*/
}
//复制选中脚本
void macroEditDialog::on_copy__clicked()
{
    int cur = ui->listWidget->currentRow();
    int cur1 = ui->listWidget_2->currentRow();
    if(cur != -1)
    {
        m_copyFlag = 0;
        m_copyId = cur;
    }
    else if(cur1 != -1)
    {
        m_copyFlag = 1;
        m_copyId = cur1;
    }
}
//粘连选中脚本
void macroEditDialog::on_paste__clicked()
{
    if(m_copyFlag == 0)
    {
        m_sucMacro.append(m_sucMacro.at(m_copyId));
        QString macroProStr = m_sucMacroPro.at(m_copyId);
        QStringList macroList = macroProStr.split('_');
        if(macroList.size() < 3)
        {
            macroList = macroProStr.split('#');
            if(macroList.size() < 3)
                return;
        }
        macroList[0] = QString::number(m_idCount);
        /*Bug 5144 - 脚本-复制、粘贴后的脚本名全部都是原脚本名-1，这里脚本名应该是按顺序1、2、3一直变化的_maxiaofeng_20200927_Begin*/
        QString str;
        QStringList tempList;
        int index = 1;
        str = macroList[2] + QString("-") + QString::number(index);
        bool bFind = false;
        while(index<100)
        {
            for(int temp = 0; temp<m_sucMacroPro.count(); temp++)
            {
                tempList = m_sucMacroPro.at(temp).split('#');
                if(str != tempList.at(2))
                {
                    bFind = false;
                }
                else
                {
                    bFind = true;
                    break;
                }
            }
            if(bFind)
            {
                index++;
                str = macroList[2] + QString("-") + QString::number(index);
            }
            else
            {
                break;
            }
        }
        macroList[2] = str;
        /*Bug 5144 - 脚本-复制、粘贴后的脚本名全部都是原脚本名-1，这里脚本名应该是按顺序1、2、3一直变化的_maxiaofeng_20200927_End*/
        macroProStr = macroList.join("#");
        m_sucMacroPro.append(macroProStr);
        m_sucsaveBitAddressList.append(object_tree_clone(m_sucsaveBitAddressList.at(m_copyId)));
        ui->listWidget->addItem(macroList[2]);
    }
    else if(m_copyFlag == 1)
    {
        m_failMacro.append(m_failMacro.at(m_copyId));
        QString macroProStr = m_failMacroPro.at(m_copyId);
        QStringList macroList = macroProStr.split('_');
        if(macroList.size() < 3)
        {
            macroList = macroProStr.split('#');
            if(macroList.size() < 3)
                return;
        }
        macroList[0] = QString::number(m_idCount);
        macroList[2] = macroList[2]+"_1";
        macroProStr = macroList.join("#");
        m_failMacroPro.append(macroProStr);
        m_failsaveBitAddressList.append(object_tree_clone(m_failsaveBitAddressList.at(m_copyId)));
        ui->listWidget->addItem(macroList[2]);
    }
    m_idList<<m_idCount;
    m_idCount++;
    //m_copyFlag =-1;
    //m_copyId = -1;
}
//导入脚本
void macroEditDialog::on_import__clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Import"),"",tr("lua Files (*.lua)"));
    if (fileName.isNull())
    {
        return;
    }
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);//只读方式
    QDataStream out(&file);
    QString sucMacro;
    out>>sucMacro;
    QString sucMacroPro;
    out>>sucMacroPro;
    QStringList macroList = sucMacroPro.split('_');
    if(macroList.size() <= 7)
    {
        macroList = sucMacroPro.split('#');
    }
    macroList[0] = QString::number(m_idCount);
    sucMacroPro = macroList.join("#");
    int i1 =0;
    QString str1 ="";
    struct object *  Addressobj  = createAddressObject(QString("saveBitAddress"),0);
    out>>i1;
    property_set_value_uint32(object_get_property(Addressobj,"Bit_or_Word"),i1);
    out>>str1;
    property_set_value_ustring(object_get_property(Addressobj,"Block"),str1.toUtf8().constData());
    out>>str1;
    property_set_value_ustring(object_get_property(Addressobj,"Link"),str1.toUtf8().constData());
    out>>str1;
    property_set_value_ustring(object_get_property(Addressobj,"No"),str1.toUtf8().constData());
    out>>i1;
    property_set_value_uint32(object_get_property(Addressobj,"index"),i1);
    out>>i1;
    property_set_value_uint32(object_get_property(Addressobj,"length"),i1);
    out>>i1;
    property_set_value_uint32(object_get_property(Addressobj,"station"),i1);
    out>>str1;
    property_set_value_ustring(object_get_property(Addressobj,"value"),str1.toUtf8().constData());
    if(macroList.size() <= 7)
    {
        QMessageBox::warning(this,
                              QObject::tr( "Warning" ),
                              QObject::tr( "File format is not correct" ),
                              QObject::tr("Ok") );
        return;
    }
    m_sucMacro.append(sucMacro);
    m_sucMacroPro.append(sucMacroPro);
    m_sucsaveBitAddressList.append(Addressobj);
    ui->listWidget->addItem(macroList[2]);
    m_idList<<m_idCount;
    m_idCount++;
    file.close();
}
//导出脚本
void macroEditDialog::on_export__clicked()
{
    int cur = ui->listWidget->currentRow();
    if(cur != -1)
    {
        m_sucsaveBitAddressList[cur];
        QString fileName = QFileDialog::getSaveFileName(this,tr("Export"),"",tr("lua Files (*.lua)"));
        if (fileName.isNull())
        {
            return;
        }
        QFile file(fileName);
        file.open(QIODevice::WriteOnly);//只写方式打开配方文件
        QDataStream in(&file);
        in<<m_sucMacro[cur];
        in<<m_sucMacroPro[cur];
        in<<object_get_property_value_type_int32(m_sucsaveBitAddressList[cur],"Bit_or_Word");
        in<<object_get_property_value_type_string(m_sucsaveBitAddressList[cur],"Block");
        in<<object_get_property_value_type_string(m_sucsaveBitAddressList[cur],"Link");
        in<<object_get_property_value_type_string(m_sucsaveBitAddressList[cur],"No");
        in<<object_get_property_value_type_int32(m_sucsaveBitAddressList[cur],"index");
        in<<object_get_property_value_type_int32(m_sucsaveBitAddressList[cur],"length");
        in<<object_get_property_value_type_int32(m_sucsaveBitAddressList[cur],"station");
        in<<object_get_property_value_type_string(m_sucsaveBitAddressList[cur],"value");
        file.close();
    }
}

//删除
void macroEditDialog::on_delecte__clicked()
{
    int cur = ui->listWidget->currentRow();
    if(cur != -1)
    {
        int r = QMessageBox::warning(this, QObject::tr("Warning"),
                                     QObject::tr("Do you want to delete the script?"),
                                     QObject::tr("Yes"),
                                     QObject::tr("No"));
        if (r == 0)
        {
            int id = m_sucMacroPro[cur].split('_').at(0).toInt();
            if(m_sucMacroPro[cur].split('_').size() < 10)
                id = m_sucMacroPro[cur].split('#').at(0).toInt();
            if(m_idList.indexOf(id) != -1)
                m_idList.removeAt(m_idList.indexOf(id));
            ui->listWidget->takeItem(cur);
            m_sucMacro.removeAt(cur);
            m_sucMacroPro.removeAt(cur);
            object_tree_destroy(m_sucsaveBitAddressList.at(cur));
            m_sucsaveBitAddressList.removeAt(cur);
        }
    }
    cur = ui->listWidget_2->currentRow();
    if(cur != -1)
    {
        int r = QMessageBox::warning(this, QObject::tr("Warning"),
                                     QObject::tr("Do you want to delete the script?"),
                                     QObject::tr("Yes"),
                                     QObject::tr("No"));
        if (r == 0)
        {
            ui->listWidget_2->takeItem(cur);
            m_failMacro.removeAt(cur);
            m_failMacroPro.removeAt(cur);
            object_tree_destroy(m_failsaveBitAddressList.at(cur));
            m_failsaveBitAddressList.removeAt(cur);
        }
    }
}

//编辑当前编译成功宏指令
void macroEditDialog::on_listWidget_doubleClicked(const QModelIndex &index)
{
   if(!m_macroE)
   {
       m_macroE =new macroDoubleclickedEditDialog((void *)(&m_links),this);
       m_macroE->initWidget();
   }
   m_macroE->setVarList(m_macroVarListPro);
   m_macroE->refreshUI(m_sucMacro.at(index.row()),m_sucMacroPro.at(index.row()),0,m_sucsaveBitAddressList.at(index.row()));
   int id = m_sucMacroPro[index.row()].split('_').at(0).toInt();
   if(m_sucMacroPro[index.row()].split('_').size() < 10)
       id = m_sucMacroPro[index.row()].split('#').at(0).toInt();
   if(m_idList.indexOf(id) != -1)
       m_idList.removeAt(m_idList.indexOf(id));
   if(m_macroE->exec() ==QDialog::Accepted)
   {
       //id检查
       int id = m_macroE->m_idedit->text().toInt();
       if(m_idList.indexOf(id) != -1)
       {
           QMessageBox::warning(this,
                                 QObject::tr( "Warning" ),
                                 QObject::tr( "This ID has already been used!" ),
                                 QObject::tr("Ok") );
           m_macroE->m_flag = 1;
       }
      if(!m_macroE->m_flag)
      {
          m_sucMacro[index.row()]= m_macroE->m_text;
          m_sucMacroPro[index.row()]= m_macroE->m_textPro;
          m_sucsaveBitAddressList[index.row()] = m_macroE->m_saveBitAddressObj;
          ui->listWidget->currentItem()->setText(m_macroE->m_nameedit->text());
          m_idList<<id;
      }
      else
      {
          m_sucMacro.removeAt(index.row());
          m_sucMacroPro.removeAt(index.row());
          m_sucsaveBitAddressList.removeAt(index.row());
          ui->listWidget->takeItem(index.row());
          m_failMacro.append(m_macroE->m_text);
          m_failMacroPro.append(m_macroE->m_textPro);
          m_failsaveBitAddressList.append(m_macroE->m_saveBitAddressObj);
          ui->listWidget_2->addItem(m_macroE->m_nameedit->text());
      }
   }
   else
   {
       m_idList<<id;
   }
}
//编辑当前未编译成功宏指令
void macroEditDialog::on_listWidget_2_doubleClicked(const QModelIndex &index)
{
    if(!m_macroE)
    {
        m_macroE =new macroDoubleclickedEditDialog((void *)(&m_links),this);
        m_macroE->initWidget();
    }
    m_macroE->setVarList(m_macroVarListPro);
    m_macroE->refreshUI(m_failMacro.at(index.row()),m_failMacroPro.at(index.row()),0,m_failsaveBitAddressList.at(index.row()));
    if(m_macroE->exec() ==QDialog::Accepted)
    {
        //id检查
        int id = m_macroE->m_idedit->text().toInt();
        if(m_idList.indexOf(id) != -1)
        {
            QMessageBox::warning(this,
                                  QObject::tr( "Warning" ),
                                  QObject::tr( "This ID has already been used!" ),
                                  QObject::tr("Ok") );
            m_macroE->m_flag = 1;
        }
       if(!m_macroE->m_flag)
       {
           m_failMacro.removeAt(index.row());
           m_failMacroPro.removeAt(index.row());
           m_failsaveBitAddressList.removeAt(index.row());
           ui->listWidget_2->takeItem(index.row());
           m_sucMacro.append(m_macroE->m_text);
           m_sucMacroPro.append(m_macroE->m_textPro);
           m_sucsaveBitAddressList.append(m_macroE->m_saveBitAddressObj);
           ui->listWidget->addItem(m_macroE->m_nameedit->text());
           m_idList<<id;
       }
       else
       {
           m_failMacro[index.row()] = m_macroE->m_text;
           m_failMacroPro[index.row()] = m_macroE->m_textPro;
           m_failsaveBitAddressList[index.row()] = m_macroE->m_saveBitAddressObj;
           ui->listWidget_2->currentItem()->setText(m_macroE->m_nameedit->text());
       }
    }
}

void macroEditDialog::on_save__clicked()
{
    /*问题修改，全局脚本数量过多时新增脚本保存有些情况会丢失脚本，yanghui，20180929，begin*/
    bool isOrNotAdd = false;
    /*问题修改，全局脚本数量过多时新增脚本保存有些情况会丢失脚本，yanghui，20180929，end*/
    /*问题修改，全局脚本数量过多时新增脚本保存有些情况会丢失脚本，yanghui，20180929，begin*/
    struct object * macroObj = NULL;
    for(int i = 0; i<m_sucMacro.size()+m_failMacro.size();i++)
    {
        if(i == 0)
        {
            macroObj  = object_first_child(m_macroObj);
        }
        else if(!isOrNotAdd)
        {
            macroObj = object_next(macroObj);
        }
        /*问题修改，全局脚本数量过多时新增脚本保存有些情况会丢失脚本，yanghui，20180929，end*/
        if(!macroObj)
        {
            /*问题修改，全局脚本数量过多时新增脚本保存有些情况会丢失脚本，yanghui，20180929，begin*/
            isOrNotAdd = true;
            /*问题修改，全局脚本数量过多时新增脚本保存有些情况会丢失脚本，yanghui，20180929，end*/
            /*问题修改,删除脚本之后保存再打开新增脚本保存会出现保存失败的情况,yanghui,20181204,begin*/
            macroObj = object_create(QString("macro%1").arg(i,3,10,QChar('0')).toUtf8().constData());
            /*问题修改,删除脚本之后保存再打开新增脚本保存会出现保存失败的情况,yanghui,20181204,end*/
            object_add_child(m_macroObj, macroObj);
            if(i < m_sucMacro.size())
            {
                /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,begin*/
                QStringList properlist = m_sucMacroPro.at(i).split('_');
                if(properlist.size() < 10)
                    properlist = m_sucMacroPro.at(i).split('#');
                /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,end*/
                object_create_property_number(macroObj,"flag",1);
                object_create_property_number(macroObj,"id",properlist.at(0).toInt());
                object_create_property_number(macroObj,"type",properlist.at(1).toInt());
                object_create_property_ustring(macroObj,"text",m_sucMacro.at(i).toUtf8().constData());
                object_create_property_ustring(macroObj,"name",properlist.at(2).toUtf8().constData());
                object_create_property_ustring(macroObj,"triggerText",properlist.at(3).toUtf8().constData());
                object_create_property_ustring(macroObj,"varlist",properlist.at(4).toUtf8().constData());
                object_create_property_number(macroObj,"cycleTime",properlist.at(5).toInt());
                object_create_property_ustring(macroObj,"fontType",properlist.at(6).toUtf8().constData());
                object_create_property_number(macroObj,"fontSize",properlist.at(7).toInt());
                object_create_property_number(macroObj,"triggerType",properlist.at(8).toInt());
                object_create_property_number(macroObj,"saveBitAddressFlag",properlist.at(9).toInt());
                object_add_child(macroObj,m_sucsaveBitAddressList.at(i));
            }
            else
            {
                /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,begin*/
                QStringList properlist = m_failMacroPro.at(i-m_sucMacro.size()).split('_');
                if(properlist.size() < 10)
                   properlist = m_failMacroPro.at(i-m_sucMacro.size()).split('#');
                /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,end*/
                object_create_property_number(macroObj,"flag",0);
                object_create_property_number(macroObj,"id",properlist.at(0).toInt());
                object_create_property_number(macroObj,"type",properlist.at(1).toInt());
                object_create_property_ustring(macroObj,"text",m_failMacro.at(i-m_sucMacro.size()).toUtf8().constData());
                object_create_property_ustring(macroObj,"name",properlist.at(2).toUtf8().constData());
                object_create_property_ustring(macroObj,"triggerText",properlist.at(3).toUtf8().constData());
                object_create_property_ustring(macroObj,"varlist",properlist.at(4).toUtf8().constData());
                object_create_property_number(macroObj,"cycleTime",properlist.at(5).toInt());
                object_create_property_ustring(macroObj,"fontType",properlist.at(6).toUtf8().constData());
                object_create_property_number(macroObj,"fontSize",properlist.at(7).toInt());
                object_create_property_number(macroObj,"triggerType",properlist.at(8).toInt());
                object_create_property_number(macroObj,"saveBitAddressFlag",properlist.at(9).toInt());
                object_add_child(macroObj,m_failsaveBitAddressList.at(i-m_sucMacro.size()));
            }
            macroObj = NULL;
        }
        else
        {
            if(i < m_sucMacro.size())
            {
                /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,begin*/
                QStringList properlist = m_sucMacroPro.at(i).split('_');
                if(properlist.size() < 10)
                   properlist = m_sucMacroPro.at(i).split('#');
                /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,end*/
                property_set_value_number(object_get_property(macroObj,"flag"),1);
                property_set_value_ustring(object_get_property(macroObj,"text"),m_sucMacro.at(i).toUtf8().constData());
                property_set_value_number(object_get_property(macroObj,"id"),properlist.at(0).toInt());
                property_set_value_number(object_get_property(macroObj,"type"),properlist.at(1).toInt());
                property_set_value_ustring(object_get_property(macroObj,"name"),properlist.at(2).toUtf8().constData());
                property_set_value_ustring(object_get_property(macroObj,"triggerText"),properlist.at(3).toUtf8().constData());
                property_set_value_ustring(object_get_property(macroObj,"varlist"),properlist.at(4).toUtf8().constData());
                property_set_value_number(object_get_property(macroObj,"cycleTime"),properlist.at(5).toInt());
                property_set_value_ustring(object_get_property(macroObj,"fontType"),properlist.at(6).toUtf8().constData());
                property_set_value_number(object_get_property(macroObj,"fontSize"),properlist.at(7).toInt());
                property_set_value_number(object_get_property(macroObj,"triggerType"),properlist.at(8).toInt());
                property_set_value_number(object_get_property(macroObj,"saveBitAddressFlag"),properlist.at(9).toInt());
                copyAddressByObj(m_sucsaveBitAddressList.at(i), object_get_child(macroObj,"saveBitAddress"));
                object_tree_destroy(m_sucsaveBitAddressList.at(i));
            }
            else
            {
                /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,begin*/
                QStringList properlist = m_failMacroPro.at(i-m_sucMacro.size()).split('_');
                if(properlist.size() < 10)
                   properlist = m_failMacroPro.at(i-m_sucMacro.size()).split('#');
                /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,end*/
                property_set_value_number(object_get_property(macroObj,"flag"),0);
                property_set_value_ustring(object_get_property(macroObj,"text"),m_failMacro.at(i-m_sucMacro.size()).toUtf8().constData());
                property_set_value_number(object_get_property(macroObj,"id"),properlist.at(0).toInt());
                property_set_value_number(object_get_property(macroObj,"type"),properlist.at(1).toInt());
                property_set_value_ustring(object_get_property(macroObj,"name"),properlist.at(2).toUtf8().constData());
                property_set_value_ustring(object_get_property(macroObj,"triggerText"),properlist.at(3).toUtf8().constData());
                property_set_value_ustring(object_get_property(macroObj,"varlist"),properlist.at(4).toUtf8().constData());
                property_set_value_number(object_get_property(macroObj,"cycleTime"),properlist.at(5).toInt());
                property_set_value_ustring(object_get_property(macroObj,"fontType"),properlist.at(6).toUtf8().constData());
                property_set_value_number(object_get_property(macroObj,"fontSize"),properlist.at(7).toInt());
                property_set_value_number(object_get_property(macroObj,"triggerType"),properlist.at(8).toInt());
                property_set_value_number(object_get_property(macroObj,"saveBitAddressFlag"),properlist.at(9).toInt());
                copyAddressByObj(m_failsaveBitAddressList.at(i-m_sucMacro.size()), object_get_child(macroObj,"saveBitAddress"));
                object_tree_destroy(m_failsaveBitAddressList.at(i-m_sucMacro.size()));
            }
        }
    }

    /*问题修改，全局脚本数量过多时新增脚本保存有些情况会丢失脚本，yanghui，20180929，begin*/
    if(!isOrNotAdd)//有新增脚本，不需要检查是否需要删除多余脚本
    {
        struct object * macroObj1;
        if(m_sucMacro.size()+m_failMacro.size() == 0)
        {
            macroObj  = object_first_child(m_macroObj);
        }
        else
        {
            macroObj = object_next(macroObj);
        }
        while(macroObj)
        {
            macroObj1 = macroObj;
            macroObj = object_next(macroObj);
            object_tree_destroy(macroObj1);
        }
    }
    /*问题修改，全局脚本数量过多时新增脚本保存有些情况会丢失脚本，yanghui，20180929，end*/

    int number = object_get_property_value_type_int32(m_macroVarListObj,"number");
    for(int i = 0; i < number;i++)
        object_remove_property(m_macroVarListObj,object_get_property(m_macroVarListObj,QString("var%1").arg(i).toLocal8Bit().data()));
    property_set_value_number(object_get_property(m_macroVarListObj,"number"),m_macroVarListPro.size());
    for(int i = 0;i<m_macroVarListPro.size();i++)
    {
        struct property * tmpPro = object_get_property(m_macroVarListObj,QString("var%1").arg(i).toLocal8Bit().data());
        if(!tmpPro)
            tmpPro = object_create_property(m_macroVarListObj,QString("var%1").arg(i).toLocal8Bit().data());
        property_set_value_ustring(tmpPro,m_macroVarListPro.at(i).toUtf8().data());
    }
    QDialog::accept();
}

/*【功能添加】_添加“拓展组件”、“脚本功能”帮助文档链接_xuxuan_20180730_Begin*/
void macroEditDialog::on_help__clicked()
{
    //globalHostApp().callHelp(89);
    QVariant var(89);
    ((UAppBase*)qApp)->_execFuncFromID(hmi_extend_help_callHelp,var);
}
/*【功能添加】_添加“拓展组件”、“脚本功能”帮助文档链接_xuxuan_20180730_End*/

void macroEditDialog::on_varTable__clicked()
{
    valueTableDialog varTable(this);
    varTable.setPro(m_macroVarListPro);
    if(varTable.exec() == QDialog::Accepted)
    {
        m_macroVarListPro = varTable.getPro();
    }
}

void macroEditDialog::deleteItem(QModelIndex index)
{
    if(sender() == ui->listWidget)
    {
        ui->listWidget_2->setCurrentRow(-1);
    }
    else
    {
        ui->listWidget->setCurrentRow(-1);
    }
}

//退出
void macroEditDialog::on_exit__clicked()
{
    QDialog::reject();
}

macroDoubleclickedEditDialog::macroDoubleclickedEditDialog(void *links, QWidget *parent):
    QDialog(parent)
{
    m_triggerAddType = NULL;
    m_flag = 1;
    m_flagType = 0;
    m_wObj = NULL;
    m_cycleTime = 50;
    if(links)
       m_links = (QMap<QString,QString>*)links;
    setWindowTitle(QObject::tr("macro edit"));
    setWindowFlags(Qt::Dialog|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);
    setWindowState(Qt::WindowMaximized);
    setMaximumSize(QApplication::desktop()->maximumSize());
    setMinimumSize(parent->size());
}

macroDoubleclickedEditDialog::~macroDoubleclickedEditDialog()
{
    if(m_wObj)
        object_tree_destroy(m_wObj);
}

void macroDoubleclickedEditDialog::initWidget()
{
        QGridLayout *mainLayout  =new QGridLayout(this);

        m_configEditor = new CodeEditor(this);
        m_font =  new QFont("宋体",15,QFont::Normal);
        m_configEditor->setFont(*m_font);
        m_configEditor->setMode(EDIT);
        QFont myfont("微软雅黑");
        this->setFont(myfont);
        mainLayout->addWidget(m_configEditor,0,0,2,12);

        QLabel *checkLabel= new QLabel(QObject::tr("compilation results:"),this);
        m_checkEdit = new QTextEdit(this);
        checkLabel->setFont(myfont);
        m_checkEdit->setFont(myfont);
        m_checkEdit->setReadOnly(true);
        mainLayout->addWidget(checkLabel,2,0,1,2);
        mainLayout->addWidget(m_checkEdit,3,0,2,2);

        m_fontStyle = new QLabel(QObject::tr("font selection"),this);
        m_fontStyleCom = new QComboBox(this);
        m_fontSize = new QLabel(QObject::tr("font size"),this);
        m_fontSizeCom = new QComboBox(this);
        m_fontStyle->setFont(myfont);
        m_fontStyleCom->setFont(myfont);
        m_fontSize->setFont(myfont);
        m_fontSizeCom->setFont(myfont);
        connect(m_configEditor,SIGNAL(editOut(QString,int)),this,SLOT(editOUT(QString,int)));
        connect(m_fontSizeCom,SIGNAL(currentIndexChanged(int)),this,SLOT(textFontChange(int)));
        connect(m_fontStyleCom,SIGNAL(currentIndexChanged(int)),this,SLOT(textFontChange(int)));
        mainLayout->addWidget(m_fontStyle,2,3,1,1);
        mainLayout->addWidget(m_fontStyleCom,2,4,1,1);
        mainLayout->addWidget(m_fontSize,2,6,1,1);
        mainLayout->addWidget(m_fontSizeCom,2,7,1,2);
        QFontDatabase fontb;
        QStringList sizeList;
        m_fontStyleCom->addItems(fontb.families());
        for(int i = 5;i<=72;i++)
            sizeList.append(QString("%1").arg(i));
        m_fontSizeCom->addItems(sizeList);
        m_idlabe = new QLabel(QObject::tr("script id"),this);
        m_idedit = new QLineEdit(this);
        m_idlabe->setFont(myfont);
        m_idedit->setFont(myfont);
        mainLayout->addWidget(m_idlabe,3,3,1,1);
        mainLayout->addWidget(m_idedit,3,4,1,1);
        m_namelabe= new QLabel(QObject::tr("script name"),this);
        m_nameedit = new QLineEdit(this);
        m_namelabe->setFont(myfont);
        m_nameedit->setFont(myfont);
        mainLayout->addWidget(m_namelabe,4,3,1,1);
        mainLayout->addWidget(m_nameedit,4,4,1,1);
        m_typelabe = new QLabel(QObject::tr("script type"),this);
        m_typebox = new QComboBox(this);
        m_typelabe->setFont(myfont);
        m_typebox->setFont(myfont);
        QStringList typelist;
        typelist<<QObject::tr("run when triggered")<<QObject::tr("start running")<<QObject::tr("run in circulation")<<QObject::tr("general");
        m_typebox->addItems(typelist);
        mainLayout->addWidget(m_typelabe,3,6,1,1);
        mainLayout->addWidget(m_typebox,3,7,1,2);
        connect(m_typebox,SIGNAL(currentIndexChanged(int)),this,SLOT(macroTypeChange(int)));
        m_cycletimelabe = new QLabel(QObject::tr("cycle Time"),this);
        m_cycleTimeMs = new QLabel(QObject::tr("ms"),this);
        m_tigglabe = new QLabel(QObject::tr("triggering conditions"),this);
        m_tiggb = new QPushButton(QObject::tr("address"),this);
        m_cycletimelabe->setFont(myfont);
        m_cycleTimeMs->setFont(myfont);
        m_tigglabe->setFont(myfont);
        m_tiggb->setFont(myfont);
        connect(m_tiggb,SIGNAL(clicked()),this,SLOT(macroTiggAddressEdit()));
        m_typeedit = new QLineEdit(this);
        m_typeedit->setFont(myfont);
        mainLayout->addWidget(m_cycletimelabe,4,6,1,1);
        mainLayout->addWidget(m_cycleTimeMs,4,8,1,1);
        mainLayout->addWidget(m_tigglabe,4,6,1,1);
        mainLayout->addWidget(m_typeedit,4,7,1,1);
        mainLayout->addWidget(m_tiggb,4,8,1,1);
        m_saveControlBit = new QCheckBox(QObject::tr("whether to use security settings:"),this);
        m_saveControlBit->setFont(myfont);
        connect(m_saveControlBit,SIGNAL(toggled(bool)),this,SLOT(saveControlbitCheckboxChange(bool)));
        m_saveControlAddressWidget = new AddressWidget(this);
        m_saveControlAddressWidget->setFont(myfont);
        m_saveControlAddressWidget->setItemName(QObject::tr("save bit address"));
        m_saveControlAddressWidget->initWidget(9);
        m_triggerTypeLabel = new QLabel(QObject::tr("trigger condition type"),this);
        m_triggerTypeLabel->setFont(myfont);
        m_triggerTypeCom = new QComboBox(this);
        m_triggerTypeCom->setFont(myfont);
        QStringList triggerList;
        triggerList<<QObject::tr("true to false")<<QObject::tr("false to true")<<QObject::tr("conditional value is changed")<<QObject::tr("value is changed");
        m_triggerTypeCom->insertItems(0,triggerList);
        mainLayout->addWidget(m_saveControlBit,2,10,1,2);
        mainLayout->addWidget(m_saveControlAddressWidget,3,10,1,2);
        mainLayout->addWidget(m_triggerTypeLabel,4,10,1,1);
        mainLayout->addWidget(m_triggerTypeCom,4,11,1,1);

         m_comfrim = new QPushButton(QObject::tr("compile check"),this);
         m_comfrim->setFont(myfont);
         connect(m_comfrim,SIGNAL(clicked()),this,SLOT(compile()));
         mainLayout->addWidget(m_comfrim,2,12,1,1);
         m_save = new QPushButton(QObject::tr("save"),this);
         m_save->setFont(myfont);
         connect(m_save,SIGNAL(clicked()),this,SLOT(mySave()));
         mainLayout->addWidget(m_save,3,12,1,1);
         m_exit = new QPushButton(QObject::tr("exit"),this);
         m_exit->setFont(myfont);
         connect(m_exit,SIGNAL(clicked()),this,SLOT(myExit()));
         mainLayout->addWidget(m_exit,4,12,1,1);
        m_treeW = new QTreeWidget(this);
        m_treeW->setFont(myfont);
        mainLayout->addWidget(m_treeW,0,12,1,1);
        initTreeW();
        connect(m_treeW,SIGNAL(itemPressed(QTreeWidgetItem *, int)),this,SLOT(itemP(QTreeWidgetItem*,int)));
        connect(m_treeW,SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),this,SLOT(itemDoubleC(QTreeWidgetItem*,int)));
        m_treeE = new QTextEdit(this);
        m_treeE->setFont(myfont);
        m_treeE->setReadOnly(true);
        mainLayout->addWidget(m_treeE,1,12,1,1);

        MyHighLighter *highlighter = new MyHighLighter(m_configEditor->document());
        m_configEditor->m_highL = highlighter;
        connect(highlighter,SIGNAL(wordListChange()),m_configEditor,SLOT(wordListChange()));
        QStringList wordList;
        for(int i = 0; i<MACROINPUTAUTOFILLEDNUM;i++)
        {
            wordList<<MACROINPUTAUTOFILLEDNAME[i];
        }
        QCompleter *completer = new QCompleter(wordList);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setCompletionRole(Qt::MatchStartsWith);
        m_configEditor->setCompleter(completer,wordList);

        mainLayout->setColumnStretch(0,4);
        mainLayout->setColumnStretch(1,16);
        mainLayout->setColumnStretch(2,1);
        mainLayout->setColumnStretch(3,4);
        mainLayout->setColumnStretch(4,4);
        mainLayout->setColumnStretch(5,1);
        mainLayout->setColumnStretch(6,4);
        mainLayout->setColumnStretch(7,8);
        mainLayout->setColumnStretch(8,4);
        mainLayout->setColumnStretch(9,1);
        mainLayout->setColumnStretch(10,4);
        mainLayout->setColumnStretch(11,8);
        mainLayout->setColumnStretch(12,16);

        mainLayout->setRowStretch(0,10);
        mainLayout->setRowStretch(1,4);
        mainLayout->setRowStretch(2,1);
        mainLayout->setRowStretch(3,1);
        mainLayout->setRowStretch(4,1);
        this->setLayout(mainLayout);
}

void macroDoubleclickedEditDialog::initTreeW()
{
    m_treeW->setHeaderLabel(QObject::tr("function"));
    //QTreeWidgetItem *items1 = new QTreeWidgetItem(m_treeW,QStringList(QObject::tr("操作组态工程参数")));
    //QTreeWidgetItem *items2 = new QTreeWidgetItem(m_treeW,QStringList(QObject::tr("操作组态工程运行")));
    //QTreeWidgetItem *items3 = new QTreeWidgetItem(m_treeW,QStringList(QObject::tr("基本库函数")));
    //QTreeWidgetItem *items4 = new QTreeWidgetItem(m_treeW,QStringList(QObject::tr("常用语句结构")));
    QTreeWidgetItem *items1 = new QTreeWidgetItem(m_treeW,QStringList(QObject::tr("operational engineering parameters")));
    QTreeWidgetItem *items2 = new QTreeWidgetItem(m_treeW,QStringList(QObject::tr("operation engineering operation")));
    QTreeWidgetItem *items3 = new QTreeWidgetItem(m_treeW,QStringList(QObject::tr("basic library function")));
    QTreeWidgetItem *items4 = new QTreeWidgetItem(m_treeW,QStringList(QObject::tr("common sentence structure")));

    //UHostApp * pApp = (UHostApp*)qApp;
    if(0)//pApp->m_language == 0)
    {
         ////1.操作组态工程参数
        for(int i = 0; i<MACROFUNCTYPE1NUM;i++)
        {
            QTreeWidgetItem *items101 = new QTreeWidgetItem(items1,QStringList(MACROFUNCTYPE1NAME_EN[i].split('@').at(0)));
            m_funcPro.insert(MACROFUNCTYPE1NAME_EN[i].split('@').at(0),MACROFUNCTYPE1NAME_EN[i].split('@').at(1));
        }
        ////2.操作组态工程运行
        for(int i = 0; i<MACROFUNCTYPE2NUM;i++)
        {
            QTreeWidgetItem *items201 = new QTreeWidgetItem(items2,QStringList(MACROFUNCTYPE2NAME_EN[i].split('@').at(0)));
            m_funcPro.insert(MACROFUNCTYPE2NAME_EN[i].split('@').at(0),MACROFUNCTYPE2NAME_EN[i].split('@').at(1));
        }
        ////4.常用语句结构
        for(int i = 0; i<MACROFUNCTYPE4NUM;i++)
        {
            QTreeWidgetItem *items401 = new QTreeWidgetItem(items4,QStringList(MACROFUNCTYPE4NAME_EN[i].split('@').at(0)));
            m_funcPro.insert(MACROFUNCTYPE4NAME_EN[i].split('@').at(0),MACROFUNCTYPE4NAME_EN[i].split('@').at(1));
        }
    }
    else
    {
        ////1.操作组态工程参数
        for(int i = 0; i<MACROFUNCTYPE1NUM;i++)
        {
            QTreeWidgetItem *items101 = new QTreeWidgetItem(items1,QStringList(MACROFUNCTYPE1NAME_CN[i].split('@').at(0)));
            m_funcPro.insert(MACROFUNCTYPE1NAME_CN[i].split('@').at(0),MACROFUNCTYPE1NAME_CN[i].split('@').at(1));
        }
        ////2.操作组态工程运行
        for(int i = 0; i<MACROFUNCTYPE2NUM;i++)
        {
            QTreeWidgetItem *items201 = new QTreeWidgetItem(items2,QStringList(MACROFUNCTYPE2NAME_CN[i].split('@').at(0)));
            m_funcPro.insert(MACROFUNCTYPE2NAME_CN[i].split('@').at(0),MACROFUNCTYPE2NAME_CN[i].split('@').at(1));
        }
        ////4.常用语句结构
        for(int i = 0; i<MACROFUNCTYPE4NUM;i++)
        {
            QTreeWidgetItem *items401 = new QTreeWidgetItem(items4,QStringList(MACROFUNCTYPE4NAME_CN[i].split('@').at(0)));
            m_funcPro.insert(MACROFUNCTYPE4NAME_CN[i].split('@').at(0),MACROFUNCTYPE4NAME_CN[i].split('@').at(1));
        }
    }

    ////3.基本库函数
    QTreeWidgetItem *items3_1 = new QTreeWidgetItem(items3 ,QStringList(QObject::tr("system correlation function")));
    QTreeWidgetItem *items3_2 = new QTreeWidgetItem(items3 ,QStringList(QObject::tr("co process")));
    QTreeWidgetItem *items3_3 = new QTreeWidgetItem(items3 ,QStringList(QObject::tr("input and output")));
    QTreeWidgetItem *items3_4 = new QTreeWidgetItem(items3 ,QStringList(QObject::tr("math")));
    QTreeWidgetItem *items3_5 = new QTreeWidgetItem(items3 ,QStringList(QObject::tr("string")));
    QTreeWidgetItem *items3_6 = new QTreeWidgetItem(items3 ,QStringList(QObject::tr("timer")));
    QTreeWidgetItem *items3_7 = new QTreeWidgetItem(items3 ,QStringList(QObject::tr("communication")));
    /*功能修改，脚本函数帮助部分中英文切换，yanghui，20180702，begin*/
    if(0)//pApp->m_language == 0)
    {
        for(int i = 0; i<MACROFUNCTYPE3NUM;i++)
        {
            if(i>=0&&i<=MACROFUNCTYPE3_1NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_1,QStringList(MACROFUNCTYPE3NAME_EN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_EN[i].split('@').at(0),MACROFUNCTYPE3NAME_EN[i].split('@').at(1));
            }
            else if(i>MACROFUNCTYPE3_1NUM && i<=MACROFUNCTYPE3_2NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_2,QStringList(MACROFUNCTYPE3NAME_EN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_EN[i].split('@').at(0),MACROFUNCTYPE3NAME_EN[i].split('@').at(1));
            }
            else if(i>MACROFUNCTYPE3_2NUM && i<=MACROFUNCTYPE3_3NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_3,QStringList(MACROFUNCTYPE3NAME_EN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_EN[i].split('@').at(0),MACROFUNCTYPE3NAME_EN[i].split('@').at(1));
            }
            else if(i>MACROFUNCTYPE3_3NUM && i<=MACROFUNCTYPE3_4NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_4,QStringList(MACROFUNCTYPE3NAME_EN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_EN[i].split('@').at(0),MACROFUNCTYPE3NAME_EN[i].split('@').at(1));
            }
            else if(i>MACROFUNCTYPE3_4NUM && i<=MACROFUNCTYPE3_5NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_5,QStringList(MACROFUNCTYPE3NAME_EN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_EN[i].split('@').at(0),MACROFUNCTYPE3NAME_EN[i].split('@').at(1));
            }
            else if(i>MACROFUNCTYPE3_5NUM && i<=MACROFUNCTYPE3_6NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_6,QStringList(MACROFUNCTYPE3NAME_EN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_EN[i].split('@').at(0),MACROFUNCTYPE3NAME_EN[i].split('@').at(1));
            }
            else
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_7,QStringList(MACROFUNCTYPE3NAME_EN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_EN[i].split('@').at(0),MACROFUNCTYPE3NAME_EN[i].split('@').at(1));
            }
        }
    }
    else
    {
        for(int i = 0; i<MACROFUNCTYPE3NUM;i++)
        {
            if(i>=0&&i<=MACROFUNCTYPE3_1NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_1,QStringList(MACROFUNCTYPE3NAME_CN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_CN[i].split('@').at(0),MACROFUNCTYPE3NAME_CN[i].split('@').at(1));
            }
            else if(i>MACROFUNCTYPE3_1NUM && i<=MACROFUNCTYPE3_2NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_2,QStringList(MACROFUNCTYPE3NAME_CN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_CN[i].split('@').at(0),MACROFUNCTYPE3NAME_CN[i].split('@').at(1));
            }
            else if(i>MACROFUNCTYPE3_2NUM && i<=MACROFUNCTYPE3_3NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_3,QStringList(MACROFUNCTYPE3NAME_CN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_CN[i].split('@').at(0),MACROFUNCTYPE3NAME_CN[i].split('@').at(1));
            }
            else if(i>MACROFUNCTYPE3_3NUM && i<=MACROFUNCTYPE3_4NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_4,QStringList(MACROFUNCTYPE3NAME_CN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_CN[i].split('@').at(0),MACROFUNCTYPE3NAME_CN[i].split('@').at(1));
            }
            else if(i>MACROFUNCTYPE3_4NUM && i<=MACROFUNCTYPE3_5NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_5,QStringList(MACROFUNCTYPE3NAME_CN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_CN[i].split('@').at(0),MACROFUNCTYPE3NAME_CN[i].split('@').at(1));
            }
            else if(i>MACROFUNCTYPE3_5NUM && i<=MACROFUNCTYPE3_6NUM)
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_6,QStringList(MACROFUNCTYPE3NAME_CN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_CN[i].split('@').at(0),MACROFUNCTYPE3NAME_CN[i].split('@').at(1));
            }
            else
            {
                QTreeWidgetItem *items = new QTreeWidgetItem(items3_7,QStringList(MACROFUNCTYPE3NAME_CN[i].split('@').at(0)));
                m_funcPro.insert(MACROFUNCTYPE3NAME_CN[i].split('@').at(0),MACROFUNCTYPE3NAME_CN[i].split('@').at(1));
            }
        }
    }
    /*功能修改，脚本函数帮助部分中英文切换，yanghui，20180702，end*/
}

void macroDoubleclickedEditDialog::refreshUI(QString macroStr, QString macroStrPro, int flag, object *saveBitObj)
{
    m_flagType = flag;
    m_textPro = macroStrPro;
    m_checkEdit->setText("");
    QStringList pro = macroStrPro.split('_');
    /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,begin*/
    if(pro.size()< 10 && macroStrPro.split('#').size() >= 10)
    {
        pro = macroStrPro.split('#');
    }
    /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,end*/
    m_idedit->setText(pro.at(0));
    m_nameedit->setText(pro.at(2));
    m_typebox->setCurrentIndex(pro.at(1).toInt());
    m_typeedit->setText(pro.at(3));
    m_triggText  = pro.at(3);
    m_tiggValueList.clear();
    for(int i = 0;i<pro.at(4).split(',').size();i++)
    {
        m_tiggValueList.append(pro.at(4).split(',').at(i));
    }
    if(pro.at(1).toInt() == 0)//触发脚本
    {
       int begin = macroStr.indexOf("--unimat_macro_trigger_begin\n")+29;
       int end = macroStr.indexOf("\n--unimat_macro_trigger_end");
       m_text = macroStr.mid(begin,end -begin);
       dealMacroText(0,m_text);
       m_configEditor->setPlainText(m_text);
       if(flag == 0)
       {
           m_cycleTime = pro.at(5).toInt();
       }
    }
    else
    {
        dealMacroText(0,macroStr);
        m_configEditor->setPlainText(macroStr);
        m_text = macroStr;
        if(flag == 0)
        {
            m_typeedit->setText(pro.at(5));
            m_cycleTime = pro.at(5).toInt();
        }
        if(pro.at(1).toInt() == 1)
            m_typeedit->setText("");
    }
    macroTypeChange(0);
    QFontDatabase fontb;
    if(1 == flag)
    {
        m_idedit->setEnabled(false);
        m_nameedit->setEnabled(false);
        m_typelabe->setVisible(false);
        m_typebox->setVisible(false);
        m_typeedit->setVisible(false);
        m_cycletimelabe->setVisible(false);
        m_cycleTimeMs->setVisible(false);
        m_tigglabe->setVisible(false);
        m_tiggb->setVisible(false);
        m_triggerTypeCom->setVisible(false);
        m_triggerTypeLabel->setVisible(false);
        //m_fontStyleCom->setCurrentIndex(fontb.families().indexOf("仿宋"));
        //m_fontSizeCom->setCurrentIndex(6);
        m_saveControlBit->setChecked(false);
        m_saveControlBit->setVisible(false);
        m_saveControlAddressWidget->setVisible(false);

    }
    else
    {
        m_saveBitAddressObj = saveBitObj;
        m_saveControlAddressWidget->setAddressData(m_saveBitAddressObj,0,1);
        m_triggerTypeCom->setCurrentIndex(pro.at(8).toInt());
        m_saveControlBit->setChecked(pro.at(9).toInt());
        saveControlbitCheckboxChange(pro.at(9).toInt());
    }
    m_fontStyleCom->setCurrentIndex(fontb.families().indexOf(pro.at(6)));
    m_fontSizeCom->setCurrentIndex(pro.at(7).toInt());
}


void macroDoubleclickedEditDialog::setVarList(QStringList list)
{
    m_valueList.clear();
    m_valueProList.clear();
    for(int i = 0; i < list.size();i++)
    {
        QStringList tmpProList = list.at(i).split(',');
        if(tmpProList.size() < 8)
            continue;
        if(m_configEditor->m_highL->m_valueList.indexOf(tmpProList.at(0)) == -1)
            m_configEditor->m_highL->m_valueList.append(tmpProList.at(0));
        if(m_valueList.indexOf(tmpProList.at(0)) == -1)
            m_valueList.append(tmpProList.at(0));
        QStringList tmpList;
        tmpList.append(QString("\"%1\"").arg(tmpProList.at(1)));//link
        tmpList.append(tmpProList.at(2));//station
        tmpList.append(QString("\"%1\"").arg(tmpProList.at(3)));//reg
        tmpList.append(tmpProList.at(4));//index
        tmpList.append(tmpProList.at(5));//len
        tmpList.append(checkDataype(tmpProList.at(6).toInt()));//datatype
        m_valueProList.append(tmpList);
    }
}

QString macroDoubleclickedEditDialog::checkDataype(int index)
{
    QString strDataType;
    switch (index) {
    case 1:
        strDataType = "DATA_TYPE_WUB";
        break;
    case 2:
        strDataType = "DATA_TYPE_WB";
        break;
    case 3:
        strDataType = "DATA_TYPE_WD";
        break;
    case 4:
        strDataType = "DATA_TYPE_DUB";
        break;
    case 5:
        strDataType = "DATA_TYPE_DB";
        break;
    case 6:
        strDataType = "DATA_TYPE_DD";
        break;
    case 7:
        strDataType = "DATA_TYPE_DF";
        break;
    case 0:
        strDataType = "DATA_TYPE_BIT";
        break;
    default:
        strDataType = "DATA_TYPE_WUB";
        break;
    }
    return strDataType;
}


QString macroDoubleclickedEditDialog::mergerExpressionToMacro(QString Expression, QString macroStr)
{
    /*功能修改，脚本功能触发脚本条件使用外部地址无法触发问题，yanghui，20180803，begin*/
    UAppBase * pApp = (UAppBase *)qApp;
    /*功能修改，脚本功能触发脚本条件使用外部地址无法触发问题，yanghui，20180803，end*/
    QString Expression1 = Expression;
    int varnum = Expression.split('[').size() - 1;
    if(varnum <=0 ||varnum != (Expression.split(']').size() - 1))
    {
        return "";
    }
    m_tiggValueList.clear();
    for(int i = 0; i < varnum; i++)
    {
        int startp = 0;
        int endp = 0;
        int p = Expression.indexOf('[',0);
        startp  = p+1;
        p = Expression.indexOf(']',p);
        endp = p -1;
        QString address = Expression.mid(startp,endp-startp+1);
        if(address.indexOf('[') != -1)
        {
            return "";
        }
        //m_tiggValueList<<address;
        QString addrStrCopy = address;
        QString linkid;
        QString reg;
        QString index;
        QString station;
        int regindex = 0;
        if(address.indexOf('/') ==-1)
        {
            linkid = "1";
            if(address.indexOf('#') ==-1)
            {
                /*功能修改，脚本功能触发脚本条件使用外部地址无法触发问题，yanghui，20180803，begin*/
                QVariant var(linkid.toInt());
                pApp->_execFuncFromID(hmi_extend_com_getLinkObj,var);
                struct object * linkObj = (struct object *) var.value<void *>();
                //struct object * linkObj= (pApp->getCurrentProject())->getLinkObjById(linkid.toInt());
                if(linkObj)
                    station = object_get_property_value_type_uint32(linkObj,"PLCID",-1);
                else
                    station= "-1";
                /*功能修改，脚本功能触发脚本条件使用外部地址无法触发问题，yanghui，20180803，end*/
            }
            else
            {
                station = address.mid(0,address.indexOf('#'));
                regindex =address.indexOf('#')+1;
            }
        }
        else
        {
            linkid = address.mid(0,address.indexOf('/'));
            if(address.indexOf('#') ==-1)
            {
                /*功能修改，脚本功能触发脚本条件使用外部地址无法触发问题，yanghui，20180803，begin*/
                QVariant var(linkid.toInt());
                pApp->_execFuncFromID(hmi_extend_com_getLinkObj,var);
                struct object * linkObj = (struct object *) var.value<void *>();
                //struct object * linkObj= (pApp->getCurrentProject())->getLinkObjById(linkid.toInt());
                if(linkObj)
                    station = object_get_property_value_type_uint32(linkObj,"PLCID",-1);
                else
                    station= "-1";
                /*功能修改，脚本功能触发脚本条件使用外部地址无法触发问题，yanghui，20180803，end*/
                 regindex  = address.indexOf('/')+1;
            }
            else
            {
                station = address.mid(address.indexOf('/')+1,address.indexOf('#') -(address.indexOf('/')+1));
                regindex =address.indexOf('#')+1;
            }
        }
        int iindex = 0;
        for(int h = address.size()-1;h >= regindex ; h--)
        {
            if((address.at(h).toLatin1() <48 || address.at(h).toLatin1() > 57) && address.at(h).toLatin1()  != '.' && address.at(h).toLatin1() != ':')
            {
                iindex = h;
                break;
            }
        }
        reg = address.mid(regindex,iindex - regindex+1);
        index = address.mid(iindex+1,address.size()-iindex - 1);
        if(reg == "LW"||reg == "LB"||reg == "RWI")
        {
            linkid = "0";
            station = "-1";
        }
        int i_index =  checkAddrToIndex(linkid.toInt(),reg,addrStrCopy);
        /*问题修改,脚本地址检查,yanghui,20200907,begin*/
        if(i_index == -1)
            return "";
        /*问题修改,脚本地址检查,yanghui,20200907,end*/
        m_tiggValueList<<linkid+"@"+reg+"@"+QString::number(i_index)+"@"+station;
        /*问题修改,当地址触发脚本配合切换画面脚本函数使用时会导致卡死的问题,yanghui,20181127,begin*/

        int regType = tools_checkAddressRegType(linkid.toInt(),reg);//checkAddressRegType(linkid.toInt(),reg);

        QString getdataStr;
        /*问题修改,db特殊地址作为触发地址时保存不过,yanghui,20200420,begin*/
        if(index.indexOf(':')!= -1)//dbx,dbw,dbw
        {
            index = "\"" + index + "\"";
        }
        /*问题修改,db特殊地址作为触发地址时保存不过,yanghui,20200420,end*/
        if(regType == 0)//bit
            getdataStr =QString("getdata(")+linkid+",\""+reg+"\","+index+","+station+",1,DATA_TYPE_BIT)";
        else
            getdataStr =QString("getdata(")+linkid+",\""+reg+"\","+index+","+station+",1,DATA_TYPE_WB)";
        /*问题修改,当地址触发脚本配合切换画面脚本函数使用时会导致卡死的问题,yanghui,20181127,end*/
        Expression.replace(startp-1,endp-startp+1+2,getdataStr);
        QString replaceStr =QString("%1").arg(0, getdataStr.size(), 10, QChar('#'));
        Expression1.replace(startp-1,endp-startp+1+2,replaceStr);//初值为0
    }
    switch (m_triggerTypeCom->currentIndex()) {
    case 0:
    {
        if(m_configEditor->isExpression(Expression) != 1)
        {
            return "";
        }
        /*问题修改,安全位和触发脚本的混合使用部分情况下（开机时）需要触发两次才生效,yanghui,20181221,,begin*/
        QString text_macro =QString("local id = %1\n").arg(m_idedit->text().toInt());
        text_macro += "local old = getTriggered(id)\nif(old == -1)\nthen\nsetTriggered(id,unimat_xxx_321)\nend\nold = getTriggered(id)\n";
        text_macro += "if(old == 0)\nthen\noldbool = false\nelse\noldbool = true\nend\n";
        text_macro += "if((oldbool == true) and (unimat_xxx_123 == false))\nthen\nsetTriggered(id,unimat_xxx_123)\n--unimat_macro_trigger_begin\n";
        text_macro +=macroStr+"\n";
        text_macro +="--unimat_macro_trigger_end\n";
        text_macro +="else\nsetTriggered(id,unimat_xxx_123)\nend\n";
        text_macro.replace("unimat_xxx_123","("+Expression+")");
        Expression1.replace("#","");
        text_macro.replace("unimat_xxx_321","("+Expression1+")");
        return text_macro;
    }
        break;
     case 1:
    {
        if(m_configEditor->isExpression(Expression) != 1)
        {
            return "";
        }
        QString text_macro =QString("local id = %1\n").arg(m_idedit->text().toInt());
        text_macro += "local old = getTriggered(id)\nif(old == -1)\nthen\nsetTriggered(id,unimat_xxx_321)\nend\nold = getTriggered(id)\n";
        text_macro += "if(old == 0)\nthen\noldbool = false\nelse\noldbool = true\nend\n";
        text_macro += "if((oldbool ==false) and (unimat_xxx_123 == true))\nthen\nsetTriggered(id,unimat_xxx_123)\n--unimat_macro_trigger_begin\n";
        text_macro +=macroStr+"\n";
        text_macro +="--unimat_macro_trigger_end\n";
        text_macro +="else\nsetTriggered(id,unimat_xxx_123)\nend\n";
        text_macro.replace("unimat_xxx_123","("+Expression+")");
        Expression1.replace("#","");
        text_macro.replace("unimat_xxx_321","("+Expression1+")");
        return text_macro;
    }
        break;
    case 2:
    {
        if(m_configEditor->isExpression(Expression) != 1)
        {
            return "";
        }
        QString text_macro =QString("local id = %1\n").arg(m_idedit->text().toInt());
        text_macro += "local old = getTriggered(id)\nif(old == -1)\nthen\nsetTriggered(id,unimat_xxx_321)\nend\nold = getTriggered(id)\n";
        text_macro += "if(old == 0)\nthen\noldbool = false\nelse\noldbool = true\nend\n";
        text_macro += "if(oldbool ~= unimat_xxx_123)\nthen\nsetTriggered(id,unimat_xxx_123)\n--unimat_macro_trigger_begin\n";
        text_macro +=macroStr+"\n";
        text_macro +="--unimat_macro_trigger_end\n";
        text_macro +="else\nsetTriggered(id,unimat_xxx_123)\nend\n";
        text_macro.replace("unimat_xxx_123","("+Expression+")");
        Expression1.replace("#","");
        text_macro.replace("unimat_xxx_321","("+Expression1+")");
        return text_macro;
    }
        break;
    case 3:
    {
        if(m_configEditor->isExpression(Expression) != 2)
        {
            return "";
        }
        QString text_macro =QString("local id = %1\n").arg(m_idedit->text().toInt());
        text_macro += "local old = getTriggered(id)\n";
        text_macro += "if(old ~= unimat_xxx_123)\nthen\nsetTriggered(id,unimat_xxx_123)\n--unimat_macro_trigger_begin\n";
        text_macro +=macroStr+"\n";
        text_macro +="--unimat_macro_trigger_end\n";
        text_macro +="else\nsetTriggered(id,unimat_xxx_123)\nend\n";
        text_macro.replace("unimat_xxx_123","("+Expression+")");
        Expression1.replace("#","");
        text_macro.replace("unimat_xxx_321","("+Expression1+")");
        return text_macro;
        /*问题修改,安全位和触发脚本的混合使用部分情况下（开机时）需要触发两次才生效,yanghui,20181221,,end*/
    }
        break;
    }
}

int macroDoubleclickedEditDialog::checkAddrToIndex(int linkid, QString reg, QString &addr)
{
    QString addrStr = addr;
    if(!m_wObj)
        m_wObj = object_create("readAdress");
    struct property *linkNamePro = object_create_property_ustring(m_wObj, "Link", "Internal storage area");
    property_set_type(linkNamePro,COMM_LINK_ADDRESS_TYPE);
    object_create_property_ustring(m_wObj, "Block", "LW");
    object_create_property_ustring(m_wObj, "No", "0");
    object_create_property_number(m_wObj, "index", 0);
    object_create_property_ustring(m_wObj, "value", "0");
    object_create_property_number(m_wObj, "length",2);
    object_create_property_number(m_wObj, "Bit_or_Word",1);
    object_create_property_number(m_wObj, "station", -1);

    /*问题修改,多个link时非第一个link时脚本触发地址提示类型错误,yanghui,20190527,begin*/
    int type = tools_checkAddressRegType(linkid,reg);
    /*问题修改,多个link时非第一个link时脚本触发地址提示类型错误,yanghui,20190527,end*/
    /*问题修改,脚本地址检查,yanghui,20200907,begin*/
    if(type == 0)
    {
        if(tools_checkAddress(m_wObj, addrStr, TYPE_BIT, 1 , RWFLAG_READ) != 0)
            return -1;
    }
    else
    {
        if(tools_checkAddress(m_wObj, addrStr, UINT_32BIT, 1 , RWFLAG_READ) != 0)
            return -1;
    }
    /*问题修改,脚本地址检查,yanghui,20200907,end*/
    return object_get_property_value_type_uint32(m_wObj,"index");
}


//flag = 0 open
//flag = 1 close
bool macroDoubleclickedEditDialog::dealMacroText(int flag, QString &Text ,bool useFlag)
{
    if(!useFlag)
    /*物联网云端变量表关联脚本功能变量表功能,yanghui,20200402,begin*/
        return true;
    /*物联网云端变量表关联脚本功能变量表功能,yanghui,20200402,end*/
    QString tmpText = Text;
    if(flag == 1)
    {
        tmpText = tmpText + " UNIMAT20200322";
        QRegExp expression("\\b[A-Za-z0-9_]+\\s*(?=\\=)");
        QRegExp expression1("[a-zA-Z0-9\\(\\,\\)]+\\s+[a-zA-Z0-9\\(\\,\\)]+");
        QRegExp expression2("[a-zA-Z0-9\\(\\,\\)]+\\s*-{2,}");
        int index = expression.indexIn(tmpText);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            if(tmpText.at(index + length +1) == '=')//"=="
            {
                index = expression.indexIn(tmpText, index + length);
                continue;
            }
            QString varName = (tmpText.mid(index,length)).trimmed();
            int tmpIndex = m_valueList.indexOf(varName);
            if(tmpIndex != -1)
            {
                int index1 = expression1.indexIn(tmpText, index +  length);
                int index1_1 = expression2.indexIn(tmpText, index +  length);
                if(index1 != -1 || index1_1 != -1)
                {
                    int length1 = 0;
                    if((index1 > index1_1 && index1_1 != -1) || index1 == -1)
                    {
                        index1 = index1_1;
                        length1 = expression2.matchedLength();
                    }
                    else
                    {
                        length1 = expression1.matchedLength();
                    }
                    QString TmpStr = tmpText.mid(index1,length1);
                    int index2 = TmpStr.indexOf((QRegExp("\\s+")));
                    int index4 = tmpText.indexOf('=',index + length);
                    int index3 = index2 + index1 -1;
                    if(index2 != -1 && index4 != -1 && index3 > index4)
                    {
                        QString valueStr = tmpText.mid(index4+1,index3-index4);
                        if(valueStr.indexOf('%') != -1)
                        {
                            valueStr.replace("%","@!*#");//避免其中存在影响格式化的%符号
                        }
                        QString tmpStr = QString("setdata(%1,%2,%3,%4,%5,%6 ,%7)")
                                .arg(m_valueProList.at(tmpIndex).at(0))//link
                                .arg(m_valueProList.at(tmpIndex).at(2))//reg
                                .arg(m_valueProList.at(tmpIndex).at(3))//index
                                .arg(m_valueProList.at(tmpIndex).at(1))//station
                                .arg(m_valueProList.at(tmpIndex).at(4))//len
                                .arg(valueStr) //data
                                .arg(m_valueProList.at(tmpIndex).at(5));//datatype
                        tmpStr.replace("@!*#","%");
                        tmpText.replace(index,index3-index+1,tmpStr);
                    }
                }
            }
            index = expression.indexIn(tmpText, index + length);
        }
        //2
        for(int i = 0;i < m_valueList.size();i++)
        {
            QRegExp expression(QString("\\b%1\\b").arg(m_valueList.at(i)));
            int index = expression.indexIn(tmpText);
            while (index >= 0)
            {
                int length = expression.matchedLength();
                QString tmpStr = QString("getdata(%1,%2,%3,%4,%5,%6)")
                        .arg(m_valueProList.at(i).at(0))//link
                        .arg(m_valueProList.at(i).at(2))//reg
                        .arg(m_valueProList.at(i).at(3))//index
                        .arg(m_valueProList.at(i).at(1))//station
                        .arg(m_valueProList.at(i).at(4))//len
                        .arg(m_valueProList.at(i).at(5));//datatype
                tmpText.replace(index,length,tmpStr);
                index = expression.indexIn(tmpText, index + length);
            }
        }
        if(tmpText.right(15) == " UNIMAT20200322")//
        {
            tmpText.remove(tmpText.size()-15,15);
        }
        Text =  tmpText;
        return true;
    }
    else//open
    {
        /*【问题修改】处理link在中文状态下存在变量表的脚本会自动转化的问题_yangjindong20201211_begin*/
        QString s = QString::fromUtf8("_\u4e00-\u9fa5");
        QString s1 = "\\bgetdata\\([a-zA-Z0-9"+s+",\"\\s_\\(\\)]+\\)";
        QString s2 = "\\bsetdata\\([a-zA-Z0-9"+s+",\\.\"\\s+\\+-/\\*%_\\(\\)]+\\)";
        QRegExp expression(s1);
        QRegExp expression1(s2);
        /*【问题修改】处理link在中文状态下存在变量表的脚本会自动转化的问题_yangjindong20201211_end*/
        QRegExp expression2("\\)\\s+[a-zA-Z\\(]+");
        QRegExp expression3("\\)\\s*-{2,}");
        int index = expression.indexIn(tmpText);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            QString varStr = (tmpText.mid(index,length)).trimmed();
            QString tmpConent = varStr.mid(varStr.indexOf('(')+1,varStr.indexOf(')')-varStr.indexOf('(')-1);
            QStringList proList = tmpConent.split(',');
            if(proList.size() == 6)
            {
                for(int i = 0;i < m_valueProList.size();i++)
                {
                    if(m_valueProList.at(i).at(0) == proList.at(0) &&
                            m_valueProList.at(i).at(2) == proList.at(1) &&
                            m_valueProList.at(i).at(3) == proList.at(2) &&
                            m_valueProList.at(i).at(1) == proList.at(3) &&
                            m_valueProList.at(i).at(4) == proList.at(4) &&
                            m_valueProList.at(i).at(5) == proList.at(5))
                    {
                        tmpText.replace(index,tmpText.indexOf(')',index)-index+1,m_valueList.at(i));
                    }
                }
            }
            index = expression.indexIn(tmpText, index + 1);
        }
        index = expression1.indexIn(tmpText);
        while (index >= 0)
        {
            int length = expression1.matchedLength();
            int index1 = expression2.indexIn(tmpText,index);
            int index1_1 = expression3.indexIn(tmpText,index);
            if((index1 > index1_1 && index1_1 != -1) || index1 == -1)
                index1 = index1_1;
            if(index1 != -1)
            {
                int index2 = index1+1;
                if(index2 != -1)
                {
                    length = index2 - index;
                }
            }
            QString varStr = (tmpText.mid(index,length)).trimmed();
            QString tmpConent = varStr.mid(varStr.indexOf('(')+1,varStr.length()-1-varStr.indexOf('(')-1);
            QStringList proList = tmpConent.split(',');
            if(proList.size() >= 7)
            {
                for(int i = 0;i < m_valueProList.size();i++)
                {
                    if(m_valueProList.at(i).at(0) == proList.at(0) &&
                            m_valueProList.at(i).at(2) == proList.at(1) &&
                            m_valueProList.at(i).at(3) == proList.at(2) &&
                            m_valueProList.at(i).at(1) == proList.at(3) &&
                            m_valueProList.at(i).at(4) == proList.at(4) &&
                            m_valueProList.at(i).at(5) == proList.at(proList.size()-1))
                    {
                        QStringList tmpList1= proList.mid(5,proList.size()-6);
                        QString tmpDataStr = tmpList1.join(",");
                        QString tmpStr = m_valueList.at(i) + QString(" =%1").arg(tmpDataStr);
                        tmpText.replace(index,length,tmpStr);
                    }
                }
            }
            index = expression1.indexIn(tmpText, index + 1);
        }
        Text = tmpText;
        return true;
    }
}

void macroDoubleclickedEditDialog::editOUT(QString content,int flag)
{
    //m_linedit->setText(content);
    m_checkEdit->setText(content);
    m_flag = flag;
}

void macroDoubleclickedEditDialog::myExit()
{
    QDialog::reject();
}

void macroDoubleclickedEditDialog::mySave()
{
    QString tmpText = m_configEditor->toPlainText();
    dealMacroText(1,tmpText);
    m_configEditor->compileMacro(tmpText);//检查脚本语法问题
    if(m_typebox->currentIndex() == 0)//触发
    {
        QString text_reslut = mergerExpressionToMacro(m_typeedit->text(),tmpText);
        if(text_reslut != "")
            m_text = text_reslut;
        else
        {
            m_checkEdit->append("please check  conditional expression of the macro");
            return ;
        }
    }
    else
    {
        m_text = tmpText;
    }
    if(m_flagType == 0)
    {
        QString tmpName = m_nameedit->text();
        if(tmpName.indexOf('#'))
            tmpName.remove('#');
        /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,begin*/
        if(m_typebox->currentIndex() != 0)
           m_textPro = QString("%1#%2#").arg(m_idedit->text().toInt()).arg(m_typebox->currentIndex()) + tmpName +"#"+m_triggText;
        else
           m_textPro = QString("%1#%2#").arg(m_idedit->text().toInt()).arg(m_typebox->currentIndex()) + tmpName +"#"+m_typeedit->text();
        QString varlist ="";
        for(int i= 0;i<m_tiggValueList.size();i++)
        {
            if(i == 0)
                varlist+=m_tiggValueList.at(i);
            else
                varlist+=","+m_tiggValueList.at(i);
        }
        if(m_typebox->currentIndex() == 2)
            m_cycleTime = m_typeedit->text().toInt();
        m_textPro +="#"+varlist+"#"+QString::number(m_cycleTime)+"#"+m_fontStyleCom->currentText()+"#"+QString::number(m_fontSizeCom->currentIndex())
                +"#"+ QString::number(m_triggerTypeCom->currentIndex()) +"#"+QString::number(m_saveControlBit->isChecked());
        if(m_textPro.split('#').size() != 10)
        {
            m_checkEdit->append("Please remove the extra #");
            return ;
        }
        /*问题修改,4x_bit地址作为触发地址时异常显示,yanghui,20200417,end*/
        if(m_saveControlBit->isChecked())
        {
            QString MAddr1 = m_saveControlAddressWidget->getMonitorAddress();
            int ret1 = tools_checkAddress(m_saveBitAddressObj, MAddr1, TYPE_BIT, 1, RWFLAG_READ);
            if (ret1 < 0)
                return;
        }
    }
    QDialog::accept();
}

void macroDoubleclickedEditDialog::compile()
{
    QString tmpStr = m_configEditor->toPlainText();
    dealMacroText(1,tmpStr);
    m_configEditor->compileMacro(tmpStr);
}

void macroDoubleclickedEditDialog::itemP(QTreeWidgetItem *item, int cm)
{
    int flag = 0;
    QMap <QString, QString>::Iterator it =m_funcPro.begin();
    while(it != m_funcPro.end())
    {
        if(item->text(cm) == it.key())
        {
            m_treeE->setText((it.value()).split('#').at(0));
            flag = 1;
            break;
        }
        it++;
    }
    if(0 == flag)
    {
        m_treeE->setText("");
    }
}

void macroDoubleclickedEditDialog::itemDoubleC(QTreeWidgetItem *item, int cm)
{
    QMap <QString, QString>::Iterator it =m_funcPro.begin();
    QStringList tempList;
    for(int i = 0;i<MACROFUNCTYPE4NUM*2;i++)
    {
        tempList<<MACROFUNCTYPE4NAME_EDIT[i];
    }
    while(it != m_funcPro.end())
    {
        if(item->text(cm) == it.key())
        {
            treeItemDoubleclickedEditDialog itemdoubleP;
            itemdoubleP.initWidget();
            itemdoubleP.refreshUI(it.value(),m_links);
            if(itemdoubleP.exec() ==QDialog::Accepted)
            {
                if(tempList.indexOf(item->text(cm)) == -1)//函数类
                {
                    QString funcStr = it.key() + "(" + itemdoubleP.m_parameter+")";
                    m_configEditor->insertPlainText(funcStr);
                }
                else
                {
                    int index =tempList.indexOf(item->text(cm));
                    if(index >= MACROFUNCTYPE4NUM)
                        index = index - MACROFUNCTYPE4NUM;
                    if(index < 5)
                        m_configEditor->insertPlainText(MACROFUNCTYPE4NAME_EDIT2[index]);
                }
            }
            break;
        }
        it++;
    }
}

void macroDoubleclickedEditDialog::macroTypeChange(int index)
{
    if(m_typebox->currentIndex() == 0)//chufa
    {
        m_tigglabe->setVisible(true);
        m_tiggb->setVisible(true);
        m_triggerTypeCom->setVisible(true);
        m_triggerTypeLabel->setVisible(true);
        m_cycletimelabe->setVisible(false);
        m_cycleTimeMs->setVisible(false);
        m_typeedit->setVisible(true);
        m_typeedit->setText(m_triggText);
    }
    else if(m_typebox->currentIndex() == 2)//cycle
    {
        m_tigglabe->setVisible(false);
        m_tiggb->setVisible(false);
        m_triggerTypeCom->setVisible(false);
        m_triggerTypeLabel->setVisible(false);
        m_cycletimelabe->setVisible(true);
        m_cycleTimeMs->setVisible(true);
        m_typeedit->setVisible(true);
        m_typeedit->setText(QString::number(m_cycleTime));
    }
    else
    {
        m_tigglabe->setVisible(false);
        m_tiggb->setVisible(false);
        m_triggerTypeCom->setVisible(false);
        m_triggerTypeLabel->setVisible(false);
        m_cycletimelabe->setVisible(false);
        m_cycleTimeMs->setVisible(false);
        m_typeedit->setVisible(false);
    }
}

void macroDoubleclickedEditDialog::macroTiggAddressEdit()
{
    if(!m_triggerAddType)
        m_triggerAddType = new triggerAddrTypeDialog(this);
    if(QDialog::Accepted == m_triggerAddType->exec())
    {
        UInputAddressDialog dialog(2, this);
        if(!m_wObj)
        {
            m_wObj = object_create("readAdress");
            struct property *linkNamePro = object_create_property_ustring(m_wObj, "Link", "Internal storage area");
            property_set_type(linkNamePro,COMM_LINK_ADDRESS_TYPE);
            object_create_property_ustring(m_wObj, "Block", "LW");
            object_create_property_ustring(m_wObj, "No", "0");
            object_create_property_number(m_wObj, "index", 0);
            object_create_property_ustring(m_wObj, "value", "0");
            object_create_property_number(m_wObj, "length",2);
            object_create_property_number(m_wObj, "Bit_or_Word",m_triggerAddType->m_select);
            object_create_property_number(m_wObj, "station", -1);
        }
        dialog.setAddressObj(m_wObj);
        dialog.m_BitorWord = m_triggerAddType->m_select;
        dialog.m_addressLength = 1;
        if(m_triggerAddType->m_select)
            dialog.setDataType(UINT_32BIT);
        else
            dialog.setDataType(TYPE_BIT);
        dialog.setRwFlag(RWFLAG_READ);
        dialog.refreshUIfromData(1);

        if(QDialog::Accepted == dialog.exec())
        {
            QString linkName = dialog.getLinkName();
            QString blockName = dialog.u_boxType->currentText();
            QString  NoName = dialog.u_edit->text();
            //int Number = dialog.m_addressNumber;
            //获取地址显示文本
            QString showAddr =  tools_getShowAddrText(linkName, blockName, NoName, dialog.m_station);
            int ret =0;
            if(m_triggerAddType->m_select)
            {
                ret = tools_checkAddress(m_wObj, showAddr, UINT_32BIT, 1 , RWFLAG_READ);
            }
            else
            {
                ret = tools_checkAddress(m_wObj, showAddr, TYPE_BIT, 1 , RWFLAG_READ);
            }
            if (ret < 0)
            {
                object_tree_destroy(m_wObj);
                m_wObj = NULL;
                return;
            }
            showAddr = "[" + showAddr + "]";
            m_typeedit->insert(showAddr);
        }
        object_tree_destroy(m_wObj);
        m_wObj = NULL;
    }
}

void macroDoubleclickedEditDialog::textFontChange(int)
{
    m_font->setPointSize(m_fontSizeCom->currentText().toInt());
    QString fontName = m_fontStyleCom->currentText();
    m_font->setFamily(fontName);
    m_configEditor->setFont(*m_font);
}

void macroDoubleclickedEditDialog::saveControlbitCheckboxChange(bool flag)
{
    if(flag)
    {
        m_saveControlAddressWidget->setVisible(true);
    }
    else
    {
        m_saveControlAddressWidget->setVisible(false);
    }
}


CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    m_KeyReturnFlag = 0;
    m_TipText = "";
    m_period = "";
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    //connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    //setMode(BROWSE);
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    if (m_c && m_c->popup()->isVisible()) //当弹出补全提示之后，屏蔽部分键盘消息
    {
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
        if(e->key()  == Qt::Key_Period) //"."点号输入补全提示判断
        {
            //QString curInputUnderCursor = textUnderCursor();
            QString curInputUnderCursor = textBeforeCurrentCursor();
            if("math" == curInputUnderCursor ||"string" == curInputUnderCursor )
            {
                m_period +=textBeforeCurrentCursor() + ".";
            }
        }
        QPlainTextEdit::keyPressEvent(e);
        QString completionPrefix =m_period + textBeforeCurrentCursor();
        if (completionPrefix != m_c->completionPrefix())
        {
            m_c->setCompletionPrefix(completionPrefix);
            if(m_c->completionCount() == 0)
            {
                m_c->popup()->hide();
                return;
            }
            m_c->popup()->setCurrentIndex(m_c->completionModel()->index(0, 0));
        }
        return;
    }
    else
    {
        m_period = "";
    }

    if (e->key() == Qt::Key_Return)//响应换行消息,退格补齐
    {
        QTextCursor tc;
        tc  = this->textCursor();
       int curLineNum =  tc.blockNumber();
        QStringList lineTextList;//当前行之前的所有行
        for(int i = 0 ;i < this->document()->blockCount() && i <=curLineNum ;i++)
        {
            lineTextList<<(this->document()->findBlockByNumber(curLineNum-i).text());
        }
        m_highL->EnterRuleSlot(lineTextList,m_KeyReturnFlag);
    }
    if((e->modifiers() == Qt::ControlModifier) && (e->key() == Qt::Key_I))
    {
        QTextCursor tc;
        tc  = this->textCursor();
        int start =  tc.selectionStart();
        int end =    tc.selectionEnd();
        int startLineNum = -1;
        int endLineNum = -1;
        if(start != end)
        {
            QStringList lineTextList;//所有行
            for(int i = 0 ;i < this->document()->blockCount();i++)
            {
                lineTextList<<(this->document()->findBlockByNumber(i).text());
            }
            //huoquxuanzhonghanghaoint
            int  charNum = 0;
            int  spaceNum = 0;
            for(int i = 0;i < lineTextList.size();i++)
            {
                if(start>=charNum && start <charNum+lineTextList.at(i).size()+1)
                    startLineNum = i;
                if(end>=charNum && end <charNum+lineTextList.at(i).size()+1)
                    endLineNum = i;
                charNum = charNum+lineTextList.at(i).size()+1;//+1=/n
                if(endLineNum != -1)
                    break;
            }
            for(int j = startLineNum;j <= endLineNum;j++)
            {
                spaceNum = 0;
                int curLineNum = j;
                if(curLineNum > 0)
                {
                    curLineNum = curLineNum -1;
                    QStringList lineTextListTmp;//当前行之前的所有行
                    for(int i = 0 ;i < lineTextList.size() && i <=curLineNum ;i++)
                    {
                        lineTextListTmp<<lineTextList[curLineNum-i];
                    }
                    m_highL->EnterRuleSlot(lineTextListTmp,spaceNum);
                }
                lineTextList[j] = lineTextList[j].trimmed();
                for(int i = 0;i < spaceNum;i++)
                {
                    lineTextList[j] = " "+lineTextList[j];
                }
                if(spaceNum >= 2 && (lineTextList.at(j).trimmed().left(3) == "end" ||
                                     lineTextList.at(j).trimmed().left(4) == "then" ||
                                     lineTextList.at(j).trimmed().left(4) == "else" ||
                                     lineTextList.at(j).trimmed().left(2) == "do" ))
                {
                    lineTextList[j] = lineTextList[j].remove(0,2);
                }
            }
            QString textTMP = lineTextList.join("\r\n");
            this->setPlainText(textTMP);
            return;
        }
    }
    if(e->key()  == Qt::Key_Period) //"."点号输入补全提示判断
    {
        //排除os./io.
        QString curInputUnderCursor = textUnderCursor();
        if("os" != curInputUnderCursor && "io" != curInputUnderCursor )
        {
            ////"."点号输入补全提示
            QTextCursor tc;
            tc  = this->textCursor();
            QRect cr = cursorRect(tc);
            m_c->setCompletionPrefix("");
            cr.setWidth(m_c->popup()->sizeHintForColumn(0) + m_c->popup()->verticalScrollBar()->sizeHint().width());
            m_c->complete(cr);
            return;
        }
        else
        {
            m_period =  curInputUnderCursor +".";
            QTextCursor tc;
            tc  = this->textCursor();
            QRect cr = cursorRect(tc);
            m_c->setCompletionPrefix(m_period);
            cr.setWidth(m_c->popup()->sizeHintForColumn(0) + m_c->popup()->verticalScrollBar()->sizeHint().width());
            m_c->complete(cr);

            if(m_c->completionCount() == 0)
            {
                m_c->popup()->hide();
            }
            else
            {
                m_c->popup()->setCurrentIndex(m_c->completionModel()->index(0, 0));
            }
        }
     }

    QPlainTextEdit::keyPressEvent(e);

    /*测试用，yanghui，0831*/
    textBeforeCurrentCursor();
    /*测试用，yanghui，0831*/
    //当输入连体字符串大与3个的时候开始联想
    QString curInputUnderCursor = textBeforeCurrentCursor();
     if(curInputUnderCursor.size() >= 3)
    {
        QTextCursor tc;
        tc  = this->textCursor();
        QRect cr = cursorRect(tc);
        m_c->setCompletionPrefix(curInputUnderCursor);
        cr.setWidth(m_c->popup()->sizeHintForColumn(0) + m_c->popup()->verticalScrollBar()->sizeHint().width());
        m_c->complete(cr);

        if(m_c->completionCount() == 0)
        {
            m_c->popup()->hide();
        }
        else
        {
            m_c->popup()->setCurrentIndex(m_c->completionModel()->index(0, 0));
        }
    }

    if (e->key() == Qt::Key_D)//end,退格控制
    {
        QTextCursor tc;
        tc  = this->textCursor();
        QString  curLineText = tc.block().text();
        QString curInputUnderCursor = textUnderCursor();
        if("end" == curInputUnderCursor && curLineText.indexOf(QRegExp("[A-Za-z0-9_]"))>1 && curLineText.trimmed() == "end")//输入为end且当前行前有两个以上空格
        {
            QTextCursor tc1 = this->textCursor();
            tc1.movePosition(QTextCursor::StartOfLine);
            tc1.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,2);
            tc1.removeSelectedText();
            tc1.movePosition(QTextCursor::EndOfLine);
            this->setTextCursor(tc1);
        }
    }
    if (e->key() == Qt::Key_E)//else,退格控制
    {
        QTextCursor tc;
        tc  = this->textCursor();
        QString  curLineText = tc.block().text();
        QString curInputUnderCursor = textUnderCursor();
        if("else" == curInputUnderCursor && curLineText.indexOf(QRegExp("[A-Za-z0-9_]"))>1 && curLineText.trimmed() == "else")//连体输入为else且当前行前有两个以上空格
        {
            QTextCursor tc1 = this->textCursor();
            tc1.movePosition(QTextCursor::StartOfLine);
            tc1.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,2);
            tc1.removeSelectedText();
            tc1.movePosition(QTextCursor::EndOfLine);
            this->setTextCursor(tc1);
        }
    }
    if (e->key() == Qt::Key_N)//then,退格控制
    {
        QTextCursor tc;
        tc  = this->textCursor();
        QString  curLineText = tc.block().text();
        QString curInputUnderCursor = textUnderCursor();
        if("then" == curInputUnderCursor && curLineText.indexOf(QRegExp("[A-Za-z0-9_]"))>1 && curLineText.trimmed() == "then")//连体输入为then且当前行前有两个以上空格
        {
            QTextCursor tc1 = this->textCursor();
            tc1.movePosition(QTextCursor::StartOfLine);
            tc1.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,2);
            tc1.removeSelectedText();
            tc1.movePosition(QTextCursor::EndOfLine);
            this->setTextCursor(tc1);
        }
    }
    if (e->key() == Qt::Key_Return)//响应换行消息,退格补齐
    {
        QString input_ ="";
        for(int i = 0;i<m_KeyReturnFlag;i++)
        {
            input_=input_ + " ";
        }
        this->insertPlainText(input_);
    }
    if(e->key() == Qt::Key_ParenLeft || e->key() == Qt::Key_BraceLeft || e->key() == Qt::Key_QuoteDbl)//"(""{"输入补全
    {
        QTextCursor tc;
        tc  = this->textCursor();
        if(e->key() == Qt::Key_ParenLeft)
            tc.insertText(")");
        else if(e->key() == Qt::Key_BraceLeft )
            tc.insertText("}");
        else if(e->key() == Qt::Key_QuoteDbl)
        {
            tc.insertText("\"");
        }
        tc.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,1);
        this->setTextCursor(tc);
    }
}

bool CodeEditor::event(QEvent *e)
{
    if(e->type() == QEvent::ToolTip)
    {
        if(m_TipText != "")
           this->setToolTip(m_TipText);
    }
    QPlainTextEdit::event(e);
}

//函数提示消息，双击然后拖动鼠标停留
void CodeEditor::mouseDoubleClickEvent(QMouseEvent *e)
{
    QTextCursor tc;
    tc  = this->textCursor();
    QString curText = tc.block().text();
    int index = tc.positionInBlock();
    if(index != 0)
    {
        if(m_highL->TipRuleSlot(curText,index))
        {
            m_TipText = curText;
        }
        else
        {
            m_TipText = "";
        }
    }
    QPlainTextEdit::mousePressEvent(e);
}

//单击取消提示
void CodeEditor::mousePressEvent(QMouseEvent *e)
{
     if(m_TipText != "")
     {
         this->setToolTip("");
         m_TipText = "";
     }
     QPlainTextEdit::mousePressEvent(e);
}

QString CodeEditor::textUnderCursor()
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

QString CodeEditor::textBeforeCurrentCursor()
{
    QTextCursor tc;
    tc  = this->textCursor();
   QString lineText = tc.block().text();
   int index = tc.positionInBlock() - 1;
  if(lineText == "")
      return "";
    int index2 = index;
    int i;
    for(i =index ; i>=0;i--)
    {
        char curchar =  lineText.at(i).toLatin1();
        if(curchar != '_'  &&  (curchar < '0' || curchar >'9') && (curchar < 'a' || curchar >'z') && (curchar < 'A' || curchar >'Z'))
        {
            index2 =i;
            break;
        }
    }
    if(i == -1)
    {
        index2 = 0;
    }
    else if(index - index2 != 0)
    {
        index2++;
    }
    return lineText.mid(index2,index - index2+1);
}

void CodeEditor::setCompleter(QCompleter *completer, QStringList list)
{
    m_wordlist = list;
    m_c = completer;
    m_c->setWidget(this);
    m_c->setCompletionMode(QCompleter::PopupCompletion);
    m_c->setCaseSensitivity(Qt::CaseInsensitive);
    connect(m_c, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

void CodeEditor::insertCompletion(const QString& completion)//补全提示选择之后的插入动作
{
    if (m_c->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_c->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

int CodeEditor::isExpression(QString str)
{
    str = "getisExpression(type("+str +"))";
    lua_State *L;
    L =  luaL_newstate();
    luaL_openlibs(L);
    lua_pushcfunction(L, l_HMIgetisExpression);    // 将C函数转换为Lua的"function"并压入虚拟栈。
    lua_setglobal(L, "getisExpression");    // 弹出栈顶元素，并在Lua中用名为"mysin"的全局变量存储。l_HMIgetData
    lua_pushcfunction(L,l_HMIgetDataOffline);
    lua_setglobal(L, "getdata");
    if(luaL_dostring(L, str.toLatin1().constData()))    // 执行Lua命令。
        printf("Failed to invoke.\n");
    lua_close(L);
    return HMI_UNIMAT_ISEXPRESSION;
}

bool CodeEditor::checkTriggerAddressType(QString text)
{
    int offset = 0;
    int leftOffset = 0;
    int rightOffset = 0;
    bool flag = false;
    while(text.indexOf("getdata",offset) != -1)
    {
        int index = text.indexOf("getdata",offset);
        for(int i = index + 6; i < text.size() ; i ++ )
        {
            if(text.at(i).toLatin1()== '(')
            {
                leftOffset ++;
                flag = true;
            }
 //           else if(text.at(i).toAscii() == "")
 //           {
 //               continue;
   //         }
            else if(text.at(i).toLatin1() == ')' && flag)
            {
                rightOffset ++;
            }
            else if(!flag)
            {
                return false;
            }

            if(leftOffset == rightOffset && leftOffset != 0)/////////////////////ok
            {
                break;
            }
            if(leftOffset < rightOffset)
            {
                break;
            }
        }
    }
}

void CodeEditor::compileMacro(QString text)
{
    QString luaContent = text;
    lua_State *L;
    L =  luaL_newstate();
    luaL_openlibs(L);
   int flag =luaL_loadstring(L,luaContent.toLatin1().constData());
   int errorFlag = 0;
   QString errorSrt;
   if(flag != 0)
   {
       QString errStr = QString(QByteArray(lua_tostring(L,-1)));
       if(-1 != errStr.indexOf("]:"))
       {
           errStr.remove(0,errStr.indexOf("]:")+1);
           if(errStr.contains("'=' or 'in' expected near '('"))
           {
               QStringList tmpList = text.split('\n');
               int lineNum = errStr.mid(1,errStr.indexOf(':',1)-1).toInt() - 1 ;
               if(lineNum >= 0 && tmpList.at(lineNum).indexOf("for") != -1)
                   errStr = QString(":%1: ").arg(lineNum) + "variable table variables cannot be used in for loop conditional statements.";
           }
           errStr = "Line" + errStr;
       }
       errorSrt = errStr;
       errorFlag = 1;
       //lua_pop(L,1);
   }
   else
   {
       errorSrt = "no error";
       errorFlag = 0;
   }
   m_highL->getFuncNameList();
   for(int i = 0; i< m_highL->m_functionNameList.size();i++)
   {
       int j ;
       for(j = 0;j<MACROINPUTAUTOFILLEDNUM + MACROFUNCCHECKNUM;j++)
       {
           if(j < MACROINPUTAUTOFILLEDNUM)
           {
               if(MACROINPUTAUTOFILLEDNAME[j].indexOf( m_highL->m_functionNameList[i]) != -1)
                   break;
           }
           else
           {
               if(MACROFUNCCHECKNAME[j - MACROINPUTAUTOFILLEDNUM].indexOf(m_highL->m_functionNameList[i]) != -1)
                   break;
           }
       }
       if(j == MACROINPUTAUTOFILLEDNUM + MACROFUNCCHECKNUM)
       {
           errorSrt+="\nwarning :function: "+ m_highL->m_functionNameList[i]+" please check spelling";
           //errorFlag = 1;
       }
   }
   emit editOut(errorSrt,errorFlag);
   lua_close(L);
}


void CodeEditor::wordListChange()
{
    QStringList testList;
    testList = m_wordlist + m_highL->m_varNameList;
    m_c->setModel(new QStringListModel(testList, this));
}


void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = this->textCursor();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(-4, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignBottom|Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
void CodeEditor::setMode(editorMode mode)
{
    if(mode == BROWSE)
    {
        this->setReadOnly(true);
        this->setStyleSheet("background:#f2f2f3;");
         //highlightCurrentLine();
    }
    else if(mode == EDIT)
    {
        this->setReadOnly(false);
        this->setStyleSheet("background:#ffffff;");
         //highlightCurrentLine();
    }
}

MyHighLighter::MyHighLighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    ////换行退格规则集合
    keywordFormat.setForeground(Qt::magenta);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns1;
    /*功能优化,脚本功能使用优化,yanghui,20200213,begin*/
    keywordPatterns1 <<"\\bif\\b"<<"\\bwhile\\b"
                     <<"\\bfor\\b"<<"\\bfunction\\b"<<"\\brepeat\\b"
                     <<"\\bend\\b"<<"\\buntil\\b";
    /*功能优化,脚本功能使用优化,yanghui,20200213,end*/
    foreach (const QString &pattern, keywordPatterns1) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        EnterRules.append(rule);
    }

    ////长停留提示消息集合
    keywordFormat.setForeground(Qt::magenta);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns2;
    for(int i = 0;i<MACROFUNCTIPNUM;i++)
    {
        keywordPatterns2 <<MACROFUNCTIPNAME[i].split('@').at(0);
        Tipstrs.append(MACROFUNCTIPNAME[i].split('@').at(1));
    }

    foreach (const QString &pattern, keywordPatterns2) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        TipRules.append(rule);
    }

    functionFormat.setFontItalic(true);        //设置斜体 //函数名
    functionFormat.setForeground(Qt::black);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    ////高亮提示规则集合
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns <<"\\bif\\b"<<"\\bthen\\b"<<"\\belse\\b"<<"\\bend\\b"
                    << "\\bfunction\\b"<< "\\breturn\\b"<< "\\blocal\\b"
                    << "\\bfor\\b"<< "\\bin\\b"<< "\\bdo\\b"<< "\\brepeat\\b"<< "\\buntil\\b"<< "\\bwhile\\b"
                    <<"\\bDATA_TYPE_BIT\\b"<<"\\bDATA_TYPE_BB\\b"<<"\\bDATA_TYPE_BUB\\b"<<"\\bDATA_TYPE_BD\\b"
                    <<"\\bDATA_TYPE_WB\\b"<<"\\bDATA_TYPE_WUB\\b"<<"\\bDATA_TYPE_WD\\b"
                    <<"\\bDATA_TYPE_DB\\b"<<"\\bDATA_TYPE_DUB\\b"<<"\\bDATA_TYPE_DD\\b"<<"\\bDATA_TYPE_DF\\b"
                    <<"\\bDATA_TYPE_STR_UNI\\b"<<"\\bDATA_TYPE_STR_ASC\\b"
                    <<"\\bCYCLE\\b"<<"\\bTRIGGER\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);//""引号字符串变色
    //rule.pattern = QRegExp("\".*\"");//   "\"([^\"]*)\""
    rule.pattern = QRegExp("\"([^\"]*)\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);


    //varFormat.setForeground(Qt::black);    //匹配=之前的变量名
    //rule.pattern = QRegExp("\\b[A-Za-z0-9_]+\\s*(?=\\=)");
    //rule.format = varFormat;
    //highlightingRules.append(rule);


    singleLineCommentFormat.setForeground(Qt::darkGreen);//--注释变色
    rule.pattern = QRegExp("--[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGreen);//多行注释变色
//    commentStartExpression = QRegExp("/\\*");
//    commentEndExpression = QRegExp("\\*/");
    commentStartExpression = QRegExp("--\\[\\[");
    commentEndExpression = QRegExp("--\\]\\]");
}

//回车之后的光标位置计算
void MyHighLighter::EnterRuleSlot(const QStringList &text, int &enterflag)
{
    int theFirstIndex = -1;//根据当前行第一个语法关键字来判断
    int curRule = -1;
    int i ;
    int j ;
    for(j = 0 ; j<text.size() && -1 == theFirstIndex; j++)
    {
        for(i = 0;i<EnterRules.size();i++)
        {
            const HighlightingRule &rule =  EnterRules.at(i);
            QRegExp expression(rule.pattern);
            int index = expression.indexIn(text.at(j));
            if(index >= 0)
            {
                if(-1 == theFirstIndex || index<theFirstIndex)
                {
                    theFirstIndex = index;
                    curRule = i;
                }
            }
        }
    }

    if(-1 != theFirstIndex)
    {
        if(curRule >= 5)
            enterflag = theFirstIndex;
        else
            enterflag = theFirstIndex + 2 ;
    }
    else
    {
        enterflag = 0;
    }
}

//函数提示规则
int MyHighLighter::TipRuleSlot(QString &text, int index1)
{
    for(int i = 0;i<TipRules.size();i++)
    {
        const HighlightingRule &rule =  TipRules.at(i);
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            if(index1>=index && index1 <=index + length)
            {
                text = Tipstrs.at(i);
                return 1;
            }
            index = expression.indexIn(text, index + length);
        }
    }
    return 0;
}

void MyHighLighter::getFuncNameList()
{
     m_functionNameList.clear();
    for(int i = 0 ;i <document()->blockCount();i++)
    {
        QString strtmp=document()->findBlockByNumber(i).text();
        QRegExp expression("\\b[A-Za-z0-9_]+(?=\\()");
        int index = expression.indexIn(strtmp);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            QString varName = (strtmp.mid(index,length)).trimmed();
            /*Bug 3157 - for循环功能描述举例时，迭代器函数ipairs少了字母i _yangjindong_20181024_begin*/
            if(m_functionNameList.indexOf(varName) == -1 && varName!="if" && varName!="while"&& varName !="until"&& varName !="ipairs" && varName !="pairs")
            {
                m_functionNameList<<varName;
            }
            /*Bug 3157 - for循环功能描述举例时，迭代器函数ipairs少了字母i _yangjindong_20181024_end*/
            index = expression.indexIn(strtmp, index + length);
        }
    }
}

void MyHighLighter::highlightBlock(const QString &text)
{
     QStringList listTmp = m_varNameList;
     m_varNameList.clear();
     m_varNameList = m_varNameList + m_valueList;
     keywordFormat.setForeground(Qt::darkRed);
     keywordFormat.setFontWeight(QFont::Bold);
     QString strtmp  =document()->toPlainText();
     QRegExp expression("\\b[A-Za-z0-9_]+\\s*(?=\\=)");//
     int index = expression.indexIn(strtmp);
     while (index >= 0)
     {
         int length = expression.matchedLength();
         QString varName = (strtmp.mid(index,length)).trimmed();
         if(m_varNameList.indexOf(varName) == -1)
         {
             m_varNameList<<varName;
         }
         //setFormat(index, length, keywordFormat);
         index = expression.indexIn(strtmp, index + length);
     }
    /*功能优化,脚本功能使用优化,yanghui,20200213,begin*/
    ////varnamelist
    QVector<HighlightingRule> VarList;
    HighlightingRule rule;
    keywordFormat.setForeground(Qt::darkGreen);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    for(int i = 0;i < m_varNameList.size();i++)
    {
       keywordPatterns << QString("\\b%1\\b").arg(m_varNameList.at(i));
    }
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        VarList.append(rule);
    }
    foreach (const HighlightingRule &rule, VarList) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    if(listTmp.size() != m_varNameList.size())
    {
        emit wordListChange();
    }
    else
    {
        int i ;
        for(i = 0; i < listTmp.size();i++)
        {
            if(m_varNameList.indexOf(listTmp.at(i)) == -1)
            {
                break;
            }
        }
        if(i < listTmp.size())
        {
            emit wordListChange();
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);


    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                    + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}


treeItemDoubleclickedEditDialog::treeItemDoubleclickedEditDialog(QWidget *parent)
{
    m_parameter = "";
    m_parameterLayout = NULL;
    this->setWindowTitle(QObject::tr("parameter"));
    m_linkscombox = NULL;
    m_regcombox = NULL;
//    setWindowFlags(Qt::Dialog|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);
//    setWindowState(Qt::WindowMaximized);
//    QSize maxsize;
//    maxsize.setWidth(parent->size().width()*0.25);
//    maxsize.setHeight(parent->size().height()*0.5);
//    setMaximumSize(maxsize*2);
//    setMinimumSize(maxsize);
//    setMaximumSize(QApplication::desktop()->maximumSize());
//    setMinimumSize(parent->size());
}

void treeItemDoubleclickedEditDialog::initWidget()
{
    m_mainLayout  =new QGridLayout(this);
    QLabel* infolabel = new QLabel(QObject::tr("info"),this);
    m_mainLayout->addWidget(infolabel,0,0,1,1);
    m_infoEdit = new QTextEdit(this);
    m_infoEdit->setReadOnly(true);
    m_mainLayout->addWidget(m_infoEdit,1,0,1,1);
    QPushButton * savebutt = new QPushButton("save",this);
    connect(savebutt,SIGNAL(clicked()),this,SLOT(mySave()));
    m_mainLayout->addWidget(savebutt,3,0,1,1);
    this->setLayout(m_mainLayout);
}

void treeItemDoubleclickedEditDialog::refreshUI(QString pro,QMap<QString,QString>* links)
{
    //0-下拉框，1-编辑框
    //0-编辑框数字类型，1-编辑框字符串类型
    //0-必填参数，1-可选填参数（有默认参数）2-可变参数（个数不定）
    //"setdata,设置内外部地址的值#6#linkId_1_1_link1,reg_0_1_LW_LB,index_1_0_0_0,station_1_0_0_0,len_1_0_0,data_1_0_0"
    m_links = links;
    m_parameterLayout  =new QGridLayout(this);
     QStringList parlist = pro.split('#');
     if(parlist.size() != 3)
         return;
     m_infoEdit->setText(parlist.at(0));
     if(parlist.at(2).split(',').size() != parlist.at(1).toInt())
         return;
     QStringList parlist2 = parlist.at(2).split(',');
     for(int i = 0; i<parlist2.size();i++)
     {
         QStringList parlist3 = parlist2.at(i).split('_');
         QLabel * label = new QLabel(parlist3.at(0),this);
         QCheckBox * useVarCbx = new QCheckBox(QObject::tr("use var"),this);
         connect(useVarCbx,SIGNAL(clicked()),this,SLOT(UseVarcheck()));
         useVarCbx->setChecked(false);
         m_parCheckBoxList.append(useVarCbx);

         QCheckBox * defaultCbx = new QCheckBox(QObject::tr("default par"),this);
         connect(defaultCbx,SIGNAL(clicked()),this,SLOT(defaultcheck()));
         defaultCbx->setEnabled(parlist3.at(3).toInt());
         defaultCbx->setChecked(parlist3.at(3).toInt());
         m_DefaultParCheckBoxList.append(defaultCbx);

         m_parameterLayout->addWidget(label,i,0,1,1);
         m_parameterLayout->addWidget(defaultCbx,i,2,1,1);
         m_parameterLayout->addWidget(useVarCbx,i,3,1,1);
         if(parlist3.at(1) == "0")
         {
             QLineEdit * lineE = new QLineEdit(this);
             lineE->setVisible(false);
             QComboBox * combox = new QComboBox(this);
             QStringList comboxList;
             for(int j = 0;j<parlist3.size() - 5 ; j++)
             {
                 /*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180809_Begin*/
                  QString tmpStr = parlist3.at(5+j);
                  comboxList<<(tmpStr.replace('~','_'));
                  /*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180809_End*/
             }
             if(parlist3.at(0) == "link")
             {
                 m_linkscombox = combox;
                 QMap<QString,QString>::Iterator it = m_links->begin();
                 while(it != m_links->end())
                 {
                     comboxList<<(it.key()).split('#').at(0);
                     it++;
                 }
             }
             if(parlist3.at(0) == "linkid")
             {
                 m_linkscombox = combox;
                 QMap<QString,QString>::Iterator it = m_links->begin();
                 while(it != m_links->end())
                 {
                     comboxList<<(it.key()).split('#').at(1);
                     it++;
                 }
             }
             if(parlist3.at(0) == "reg")
             {
                 m_regcombox = combox;
                 comboxList<<"LB"<<"LW";
                 QStringList blockslist = (m_links->begin().value()).split(',');
                 for(int g = 1; g<blockslist.size() ;g++)
                 {
                    comboxList<<blockslist.at(g);
                 }
                 if(m_linkscombox)
                 {
                     connect(m_linkscombox,SIGNAL(currentIndexChanged(int)),this,SLOT(linkComboxChange(int)));
                 }
             }
             if(parlist3.at(0) == "DataType")
             {
                  comboxList<<"DATA_TYPE_BIT"<<"DATA_TYPE_BB"<<"DATA_TYPE_BUB"<<"DATA_TYPE_BD"
                                    <<"DATA_TYPE_WB"<<"DATA_TYPE_WUB"<<"DATA_TYPE_WD"
                                    <<"DATA_TYPE_DB"<<"DATA_TYPE_DUB"<<"DATA_TYPE_DD"<<"DATA_TYPE_DF"
                                    <<"DATA_TYPE_STR_UNI"<<"DATA_TYPE_STR_ASC";
             }
             combox->insertItems(0,comboxList);
             /*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180809_Begin*/
             m_infoStr =  parlist.at(0);
             connect(combox,SIGNAL(currentIndexChanged(QString)),this,SLOT(systemParInfoSlot(QString)));
             combox->setCurrentIndex(-1);
             combox->setCurrentIndex(parlist3.at(4).toInt());
             combox->setEnabled(!parlist3.at(3).toInt());
             /*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180809_End*/

             m_parameterLayout->addWidget(combox,i,1,1,1);
             m_parameterLayout->addWidget(lineE,i,1,1,1);
             m_parItemList.append((void *)combox);
             m_parItemTypeList.append(0);
             m_parTypeList.append(parlist3.at(2).toInt());
             m_parUseVarItemMap.insert(i,(void *)lineE);
         }
         else if(parlist3.at(1) == "1")
         {
               QLineEdit * lineE = new QLineEdit(this);
               lineE->setText(parlist3.at(4));
               lineE->setEnabled(!parlist3.at(3).toInt());
               m_parameterLayout->addWidget(lineE,i,1,1,1);
               m_parItemList.append((void *)lineE);
               m_parItemTypeList.append(1);
               m_parTypeList.append(parlist3.at(2).toInt());
         }
     }
     m_mainLayout->addLayout(m_parameterLayout,2,0,1,1);
}

/*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180809_Begin*/
void treeItemDoubleclickedEditDialog::systemParInfoSlot(QString value)
{
    for(int i = 0;i<MACROSYSTEMPARNUM;i++)
    {
        if(value == MACROSYSTEMPAR[i])
        {
            if(tools_getSystemCurrentLanguage() == 0)
            {
                m_infoEdit->setText(m_infoStr+MACROSYSTEMPAR_EN[i]);
                break;
            }
            else
            {
                m_infoEdit->setText(m_infoStr+MACROSYSTEMPAR_CN[i]);
                break;
            }
        }
    }
}
/*【功能优化】_脚本功能添加命令到下拉框中_yangjindong_20180809_End*/

void treeItemDoubleclickedEditDialog::mySave()
{
    QString comboxValue = "";
    m_parameter = "";
    for(int i = 0;i<m_parItemList.size();i++)
    {
        if(((QCheckBox *)m_DefaultParCheckBoxList.at(i))->isChecked() && ((QCheckBox *)m_DefaultParCheckBoxList.at(i))->isEnabled())
        {
            break;
        }
        if(m_parItemTypeList.at(i) == 0)
        {
            if(((QCheckBox *)m_parCheckBoxList.at(i))->isChecked())
            {
               comboxValue =  ((QLineEdit *)m_parUseVarItemMap.value(i))->text();
               m_parTypeList[i] = 0;
            }
            else
            {
                comboxValue = ((QComboBox *)m_parItemList.at(i))->currentText();
            }
            if(m_parTypeList.at(i) ==1)
                m_parameter +="\""+ comboxValue +"\"" + ",";
            else
                m_parameter +=comboxValue + ",";
        }
        else if(m_parItemTypeList.at(i) == 1)
        {
            if(m_parTypeList.at(i) ==1 && !((QCheckBox *)m_parCheckBoxList.at(i))->isChecked())
                m_parameter +="\""+ ((QLineEdit *)m_parItemList.at(i))->text()+"\""+ ",";
            else
                m_parameter +=((QLineEdit *)m_parItemList.at(i))->text()+ ",";
        }
    }
    if(m_parameter != "")
        m_parameter.remove(m_parameter.size()-1,1);
    QDialog::accept();
}

void treeItemDoubleclickedEditDialog::UseVarcheck()
{
   int id = -1;
   void * sendmsg=sender();
   QCheckBox * sendCheckB = (QCheckBox *)sender();
   for(int i = 0;i<m_parCheckBoxList.size();i++)
   {
       if(sendmsg == m_parCheckBoxList.at(i))
       {
           id = i;
           break;
       }
   }
   if(id != -1)
   {
       if(m_parItemTypeList.at(id) == 1)
           return;
       if(sendCheckB->isChecked())
       {
           ((QWidget *)(m_parItemList.at(id)))->setVisible(false);
           ((QLineEdit *)m_parUseVarItemMap.value(id))->setVisible(true);
           ((QCheckBox * )m_DefaultParCheckBoxList.at(id))->setEnabled(false);
       }
       else
       {
           ((QWidget *)(m_parItemList.at(id)))->setVisible(true);
           ((QLineEdit *)m_parUseVarItemMap.value(id))->setVisible(false);
           ((QCheckBox * )m_DefaultParCheckBoxList.at(id))->setEnabled(true);
       }
   }
}

void treeItemDoubleclickedEditDialog::defaultcheck()
{
    int id = -1;
    void * sendmsg=sender();
    QCheckBox * sendCheckB = (QCheckBox *)sender();
    for(int i = 0;i<m_DefaultParCheckBoxList.size();i++)
    {
        if(sendmsg == m_DefaultParCheckBoxList.at(i))
        {
            id = i;
            break;
        }
    }
    if(id != -1)
    {
        if(sendCheckB->isChecked())
        {
            ((QWidget *)(m_parItemList.at(id)))->setEnabled(false);
        }
        else
        {
            ((QWidget *)(m_parItemList.at(id)))->setEnabled(true);
        }
    }
}

void treeItemDoubleclickedEditDialog::linkComboxChange(int index)
{
    QString curLinksName = m_linkscombox->currentText();
    QMap<QString,QString>::Iterator it = m_links->begin();
    while(it != m_links->end())
    {
        if(it.key().split('#').at(0) == curLinksName || it.key().split('#').at(1) == curLinksName)
        {
            m_regcombox->clear();
            QStringList regcomList;
            QStringList linksRegList = (it.value()).split(',');
            regcomList<<"LB"<<"LW";
            for(int i = 1; i < linksRegList.size() ; i++)
            {
                regcomList <<linksRegList.at(i);
            }
            m_regcombox->insertItems(0,regcomList);
            break;
        }
        it++;
    }
}


triggerAddrTypeDialog::triggerAddrTypeDialog(QWidget *parent)
{
    m_select = 1;
    this->setFixedSize(120,130);
    this->setWindowTitle(QObject::tr("Type"));
    m_mainLayout  =new QGridLayout(this);
    QLabel* infolabel = new QLabel(QObject::tr("info"),this);
    m_mainLayout->addWidget(infolabel,0,0,1,1);
    m_bitCheck = new QCheckBox(QObject::tr("use bit"),this);
    m_bitCheck->setChecked(false);
    m_byteCheck = new QCheckBox(QObject::tr("use byte"),this);
    m_byteCheck->setChecked(true);
    connect(m_bitCheck,SIGNAL(clicked(bool)),this,SLOT(typeChange(bool)));
    connect(m_byteCheck,SIGNAL(clicked(bool)),this,SLOT(typeChange(bool)));
    m_mainLayout->addWidget(m_bitCheck,1,0,1,1);
    m_mainLayout->addWidget(m_byteCheck,2,0,1,1);
    QPushButton * savebutt = new QPushButton(QObject::tr("save"),this);
    connect(savebutt,SIGNAL(clicked()),this,SLOT(myOk()));
    m_mainLayout->addWidget(savebutt,3,0,1,1);
    this->setLayout(m_mainLayout);
}

void triggerAddrTypeDialog::myOk()
{
    if(m_bitCheck->isChecked())
        m_select = 0;
    else
        m_select = 1;
    QDialog::accept();
}

void triggerAddrTypeDialog::typeChange(bool flag)
{
    flag = true;
    void * sendmsg=sender();
    QCheckBox * sender_check = (QCheckBox *)sendmsg;
    if(sender_check)
    {
        if(sender_check == m_bitCheck)
        {
            m_byteCheck->setChecked(!sender_check->isChecked());
        }
        else
        {
            m_bitCheck->setChecked(!sender_check->isChecked());
        }
    }
}

valueTableDialog::valueTableDialog(QWidget *parent)
{
    m_mainLayout = new QGridLayout(this);
    this->resize(600,200);
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    QStringList header;
    header<<QObject::tr("address")<<QObject::tr("dataType")<<QObject::tr("alias")<<QObject::tr("note");
    m_table->setHorizontalHeaderLabels(header);

    this->setWindowTitle(QObject::tr("Value List"));
    m_addPtn = new QPushButton(QObject::tr("add"),this);
    m_deletePtn = new QPushButton(QObject::tr("delete"),this);
    m_okPtn = new QPushButton(QObject::tr("ok"),this);
    m_exitPtn = new QPushButton(QObject::tr("exit"),this);
    connect(m_addPtn,SIGNAL(clicked()),this,SLOT(_addSlot()));
    connect(m_deletePtn,SIGNAL(clicked()),this,SLOT(_deleteSlot()));
    connect(m_okPtn,SIGNAL(clicked()),this,SLOT(_Ok()));
    connect(m_exitPtn,SIGNAL(clicked()),this,SLOT(_exit()));
    connect(m_table,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(_addrEdit(int,int)));
    m_mainLayout->addWidget(m_table,0,0,4,1);
    m_mainLayout->addWidget(m_addPtn,0,1,1,1);
    m_mainLayout->addWidget(m_deletePtn,1,1,1,1);
    m_mainLayout->addWidget(m_okPtn,2,1,1,1);
    m_mainLayout->addWidget(m_exitPtn,3,1,1,1);
    this->setLayout(m_mainLayout);
}

void valueTableDialog::setPro(QStringList list1)
{
    m_proList = list1;

    for(int i = 0; i < m_proList.size();i++)
    {
        QStringList proList = m_proList.at(i).split(',');
        if(proList.size() < 8)
            continue;
        _ADDRESS_ tempPro;
        tempPro.block = proList.at(3);
        //tempPro.index = proList.at(4).toInt();
        tempPro.length = proList.at(5).toInt();
        tempPro.link = proList.at(1);
        tempPro.no = proList.at(4);
        tempPro.sta = proList.at(2).toInt();
        m_vAddressPro.append(tempPro);

        int rowcount = m_table->rowCount();
        m_table->setRowCount(rowcount+1);
        m_table->setItem(rowcount,2,new QTableWidgetItem(proList.at(0)));
        QString sShowText = tools_getShowAddrText(tempPro.link, tempPro.block, tempPro.no, tempPro.sta);
        QTableWidgetItem * tmpItem = new QTableWidgetItem(sShowText);
        tmpItem->setFlags(tmpItem->flags() & (~Qt::ItemIsEditable));
        m_table->setItem(rowcount,0,tmpItem);
        QComboBox *tmpbox = new QComboBox(this);
        QStringList dataTypeList;
        dataTypeList<<"1-bit"<<"16-bit unsigned"<<"16-bit signed"<<"16-bit bcd"<<"32-bit unsigned"<<"32-bit signed"<<"32-bit bcd"<<"32-bit float";
        tmpbox->addItems(dataTypeList);
        tmpbox->setCurrentIndex(proList.at(6).toInt());
        m_table->setCellWidget(rowcount,1,tmpbox);
        for(int j = 0; j < 7;j++)//
        {
            proList.removeFirst();
        }
        m_table->setItem(rowcount,3,new QTableWidgetItem(proList.join(",")));
    }
}

QStringList valueTableDialog::getPro()
{
    return m_proList;
}

void valueTableDialog::_Ok()
{
    m_proList.clear();
    QList<QString> tmpNameList;
    QList<QString> tmpAddrProList;
    for(int i = 0; i < m_vAddressPro.size();i++)
    {
        //检查地址是否正常
        int len = 1;
        QString typeText = ((QComboBox *)m_table->cellWidget(i,1))->currentText();
        if (typeText.contains("32"))
        {
            len = 2;
        }
        int dataType = 0;
        int dataTypeIndex =  ((QComboBox *)m_table->cellWidget(i,1))->currentIndex();
        if(dataTypeIndex == 0)//bit
        {
            dataType = TYPE_BIT;
        }
        else if(dataTypeIndex >= 1 && dataTypeIndex <= 3)
        {
            dataType = USHORT_16BIT;
        }
        else if(dataTypeIndex >= 4 && dataTypeIndex <= 7)
        {
            dataType = UINT_32BIT;
        }
        _ADDRESS_ tmpAddr = m_vAddressPro.at(i);
        QString tmpShowStr = tools_getShowAddrText(tmpAddr.link,tmpAddr.block,tmpAddr.no,tmpAddr.sta);
        int ret = tools_checkAddress(NULL,tmpShowStr,dataType,len,RWFLAG_RW);
        if(ret < 0)
        {
            return;
        }
        //检查是否存在名字重复项
        QString tmpName = m_table->item(i,2)->text();
        if(tmpNameList.indexOf(tmpName) != -1)
        {
            QMessageBox::warning(this,
                                 QObject::tr("Warning"),
                                 QObject::tr("Duplicate variable name!"),
                                 QObject::tr("OK"));
            return;
        }
        else
        {
            tmpNameList.append(tmpName);
        }
        //检查地址是否存在重复项
        QString addrProStr =  QString("%1,%2,%3,%4,%5,%6").arg(m_vAddressPro.at(i).link)
                .arg(m_vAddressPro.at(i).sta).arg(m_vAddressPro.at(i).block).arg(m_vAddressPro.at(i).no)
                .arg(((QComboBox *)m_table->cellWidget(i,1))->currentIndex());
        if(tmpAddrProList.indexOf(addrProStr) != -1)
        {
            QMessageBox::warning(this,
                                 QObject::tr("Warning"),
                                 QObject::tr("Duplicate address!"),
                                 QObject::tr("OK"));
            return;
        }
        else
        {
            tmpAddrProList.append(addrProStr);
        }

        //保存属性
        QString tmpStr = QString("%1,%2,%3,%4,%5,%6,%7,%8").arg(m_table->item(i,2)->text()).arg(m_vAddressPro.at(i).link)
                .arg(m_vAddressPro.at(i).sta).arg(m_vAddressPro.at(i).block).arg(m_vAddressPro.at(i).no)
                .arg(len).arg(dataTypeIndex)
                .arg(m_table->item(i,3)->text());
        m_proList.append(tmpStr);
    }
    QDialog::accept();
}

void valueTableDialog::_exit()
{
    QDialog::reject();
}

void valueTableDialog::_addSlot()
{
    int rowcount = m_table->rowCount();
    m_table->setRowCount(rowcount+1);
    QTableWidgetItem * tmpItem = new QTableWidgetItem("LW0");
    tmpItem->setFlags(tmpItem->flags() & (~Qt::ItemIsEditable));
    m_table->setItem(rowcount,0,tmpItem);
    m_table->setItem(rowcount,2,new QTableWidgetItem("name"));
    QComboBox *tmpbox = new QComboBox(this);
    QStringList dataTypeList;
    dataTypeList<<"1-bit"<<"16-bit unsigned"<<"16-bit signed"<<"16-bit bcd"<<"32-bit unsigned"<<"32-bit signed"<<"32-bit bcd"<<"32-bit float";
    tmpbox->addItems(dataTypeList);
    tmpbox->setCurrentIndex(1);
    m_table->setCellWidget(rowcount,1,tmpbox);
    m_table->setItem(rowcount,3,new QTableWidgetItem(""));

    _ADDRESS_ tempPro;
    tempPro.block = "LW";
    tempPro.index = 0;
    tempPro.length = 1;
    tempPro.link = "Internal storage area";
    tempPro.no = "0";
    tempPro.sta = 0;
    m_vAddressPro.append(tempPro);
}

void valueTableDialog::_deleteSlot()
{
    int curRow =  m_table->currentRow();
    if(curRow < 0)
        return ;
    int flag = QMessageBox::warning(this,QObject::tr("warning"),QObject::tr("delete"),
                                    QObject::tr("YES"),QObject::tr("NO"));
    if(flag == 0)
    {
        m_table->removeRow(curRow);
        m_vAddressPro.remove(curRow);
    }
}

void valueTableDialog::_addrEdit(int row, int col)
{
    if(col != 0)
        return;
    struct object* rwAddrObj;
    rwAddrObj = createAddressObject("WAddress", 1);
    _ADDRESS_ tempPro = m_vAddressPro.at(row);

    property_set_value_ustring(object_get_property(rwAddrObj, "Link"), tempPro.link.toUtf8().constData());
    property_set_value_ustring(object_get_property(rwAddrObj, "Block"), tempPro.block.toUtf8().constData());
    property_set_value_ustring(object_get_property(rwAddrObj, "No"), tempPro.no.toUtf8().constData());
    property_set_value_number(object_get_property(rwAddrObj, "station"), tempPro.sta);
    property_set_value_number(object_get_property(rwAddrObj, "length"), tempPro.length);

    UInputAddressDialog dialog(2, this);
    dialog.m_BitorWord = 1;//0-bit 1-word
    dialog.m_addressLength = 1;
    int dataType = USHORT_16BIT;
    QString typeText = ((QComboBox *)m_table->cellWidget(row,1))->currentText();
    if (typeText == "16-bit unsigned")
    {
        dataType = USHORT_16BIT;
    }
    else if (typeText == "16-bit signed")
    {
        dataType = SHORT_16BIT;
    }
    else if (typeText == "16-bit bcd")
    {
        dataType = BCD_16BIT;
    }
    else if (typeText == "32-bit unsigned")
    {
        dialog.m_addressLength = 2;
        dataType = UINT_32BIT;
    }
    else if (typeText == "32-bit signed")
    {
        dialog.m_addressLength = 2;
        dataType = INT_32BIT;
    }
    else if (typeText == "32-bit bcd")
    {
        dialog.m_addressLength = 2;
        dataType = BCD_32BIT;
    }
    else if (typeText == "32-bit float")
    {
        dialog.m_addressLength = 2;
        dataType = FLOAT_32BIT;
    }
    else if(typeText == "1-bit")
    {
        dialog.m_BitorWord = 0;
        dataType = TYPE_BIT;
    }
    dialog.setDataType(dataType);
    dialog.setAddressObj(rwAddrObj);
    dialog.setRwFlag(RWFLAG_RW);
    dialog.refreshUIfromData(1);

    if(QDialog::Accepted == dialog.exec())
    {
        tempPro.link = dialog.getLinkName();
        tempPro.block = dialog.u_boxType->currentText();
        tempPro.no = dialog.u_edit->text();
        tempPro.sta = dialog.m_station;
        if(tempPro.link == "Internal storage area")
        {
            tempPro.sta = 0;
        }
        tempPro.index = dialog.m_addressNumber;
        tempPro.length = dialog.m_addressLength;
        QString sShowText = tools_getShowAddrText(tempPro.link, tempPro.block, tempPro.no, tempPro.sta);

        m_table->item(row, col)->setData(Qt::DisplayRole,sShowText);
        m_vAddressPro.replace(row, tempPro);
    }
    object_tree_destroy(rwAddrObj);
}



void macroEditDialog::on_example_clicked()
{
   //
}
