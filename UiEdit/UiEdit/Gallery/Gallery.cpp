#include "Gallery.h"
#include "GalleryDlg.h"

Gallery::Gallery()
{
}

Gallery::~Gallery()
{
    //foreach (auto img, m_imgMap) {
    //   delete img;
    //}
    m_imgMap.clear();
}

std::string Gallery::ShowImgSelectDialog(QWidget * parent)
{
    if(!m_dlg)
    {
        m_dlg = new GalleryDlg(this,parent);
        m_dlg->InitUI();
    }
    if( QDialog::Accepted == m_dlg->exec())
    {
        return m_dlg->GetSelectPic().toStdString();
    }
    return "";
}

bool Gallery::GetImgFromPath(const std::string &sPath, std::shared_ptr<QImage> &img)
{
    if(!m_dlg)
    {
        m_dlg = new GalleryDlg(this,nullptr);
        m_dlg->InitUI();
        m_dlg->hide();
    }
    QMap<std::string,std::shared_ptr<QImage> >::Iterator it = m_imgMap.find(sPath);
    if(it != m_imgMap.end())
    {
        img = it.value();
        return true;
    }
    return false;
}

bool Gallery::UpdateImgFromPath(const std::string &sPath, std::shared_ptr<QImage> &img)
{
    if(!m_dlg)
    {
        m_dlg = new GalleryDlg(this,nullptr);
        m_dlg->InitUI();
        m_dlg->hide();
    }
    QMap<std::string,std::shared_ptr<QImage> >::Iterator it = m_imgMap.find(sPath);
    if(it != m_imgMap.end())
    {
        it.value() = img;
        return true;
    }
    return false;
}
