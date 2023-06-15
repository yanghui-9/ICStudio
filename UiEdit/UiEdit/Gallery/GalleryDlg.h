#ifndef GALLERYDLG_H
#define GALLERYDLG_H
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QMap>
#include <QComboBox>

#define ICON_WIDTH  50
#define ICON_HEIGHT 50

class QImage;
class Gallery;
class GalleryDlg : public QDialog
{
    Q_OBJECT
public:
    explicit GalleryDlg(Gallery *gallery,QWidget *parent = nullptr);

    //初始化UI显示
    void InitUI();

    //初始化图库
    void InitPic();

    //获取选中确认的图片路径
    QString GetSelectPic(){
        return m_picPath;
    }

public slots:
    //图库类型切换
    void typeChangeSlot(int index);
    //确认选中
    void selectComfirmSlot();
    //取消
    void cancleSlot();
    //新增
    void addSlot();
    //删除
    void delSlot();
    //单元格双击槽
    void itemDoubleClicked(QListWidgetItem *item);

private:
    QListWidget * m_imgListWidget = nullptr;
    QPushButton * m_confirm = nullptr;
    QPushButton * m_cancle = nullptr;
    QPushButton * m_new = nullptr;
    QPushButton * m_del = nullptr;
    QComboBox * m_typeCombox;
    QMap<QString,QList<QListWidgetItem*>> m_picMap;
    Gallery * m_gallery = nullptr;
    QString m_picPath;
};

#endif // GALLERYDLG_H
