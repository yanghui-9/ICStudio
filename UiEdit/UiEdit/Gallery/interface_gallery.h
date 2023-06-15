#ifndef INTERFACE_GALLERY_H
#define INTERFACE_GALLERY_H
#include <string>

class QImage;
class QWidget;
class interface_gallery
{
public:
    virtual ~interface_gallery(){}

    //图库接口设计
    //显示模态显示图片选择对话框，返回选择的图片图库路径
    virtual std::string ShowImgSelectDialog(QWidget *parent) = 0;

    //通过图片路径获取图片对象指针
    virtual bool GetImgFromPath(const std::string &sPath,std::shared_ptr<QImage> &img) = 0;

    //通过图片路径设置图片对象
    virtual bool UpdateImgFromPath(const std::string &sPath,std::shared_ptr<QImage> &img) = 0;
};

#endif // INTERFACE_GALLERY_H
