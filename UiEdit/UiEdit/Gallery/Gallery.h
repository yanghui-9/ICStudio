#ifndef GALLERY_H
#define GALLERY_H
#include "interface_gallery.h"
#include <QMap>

class GalleryDlg;
class QImage;
class Gallery : public interface_gallery
{
    friend class GalleryDlg;
public:
    Gallery();
    virtual ~Gallery();

    //显示模态显示图片选择对话框，返回选择的图片图库路径
    virtual std::string ShowImgSelectDialog(QWidget *parent);

    //通过图片路径获取图片对象指针
    virtual bool GetImgFromPath(const std::string &sPath,std::shared_ptr<QImage> &img);

    //通过图片路径设置图片对象
    virtual bool UpdateImgFromPath(const std::string &sPath,std::shared_ptr<QImage> &img);

private:
    GalleryDlg * m_dlg = nullptr;
    QMap<std::string,std::shared_ptr<QImage> > m_imgMap;
};

#endif // GALLERY_H
