#include "VariableTable.h"
#include "VariableTableDlg.h"

VariableTable::VariableTable()
{
}

std::string VariableTable::ShowVarSelectDialog(QWidget *parent)
{
    if(!m_dlg)
    {
        m_dlg = new VariableTableDlg(this,parent);
        m_dlg->InitUI();
    }
    if( QDialog::Accepted == m_dlg->exec())
    {
        return m_dlg->GetSelect().toLocal8Bit().data();
    }
    return "";
}

int VariableTable::GetAddrinfoFromVarName(const std::string &sPath, Protocol::AddrInfoForRW &addr)
{
    if(!m_dlg)
    {
        m_dlg = new VariableTableDlg(this,nullptr);
        m_dlg->InitUI();
        m_dlg->hide();
    }
   QMap<std::string,Protocol::AddrInfoForRW >::Iterator it = m_varMap.find(sPath);
   if(it != m_varMap.end())
   {
       addr = it.value();
       return 0;
   }
   return -1;
}
