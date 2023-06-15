#ifndef CustomComponentsDlg_H
#define CustomComponentsDlg_H
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QMap>
#include <QComboBox>

#define ICON_WIDTH 50
#define ICON_HEIGHT 50

class CustomComponentsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit CustomComponentsDlg(QWidget *parent = nullptr);

    //初始化UI显示
    void InitUI();

    //初始化
    void InitPic();

    //设置选中自定义控件
    void SetSelectItems(const QString &sItems);

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
    QMap<QString,QList<QWidget*>> m_widgetMap;
    QMap<QString,QStringList> m_selectMap;
    int m_curIndex = -1;
    QString m_picPath;
};

#endif // CustomComponentsDlg_H
