#ifndef DisplayCustomComponentsDlg_H
#define DisplayCustomComponentsDlg_H
#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMap>
#include <QComboBox>


class MainWindow;
class DisplayCustomComponentsDlg : public QDialog
{
    Q_OBJECT
public:
    explicit DisplayCustomComponentsDlg(MainWindow *mainW, QWidget *parent = nullptr);

    //初始化UI显示
    void InitUI();

    //显示
    void UShow();

    //加载
    void LoadCombineItems(const QString &sList);

public slots:
    //单元格双击槽
    void itemClicked(QListWidgetItem *item);

private:
    QListWidget * m_imgListWidget = nullptr;
    QMap<QString,QList<QListWidgetItem*>> m_picMap;
    QMap<QString,QList<QWidget*>> m_widgetMap;
    int m_curIndex = -1;
    QString m_sItemList;
    MainWindow * m_mainW = nullptr;
};

#endif // DisplayCustomComponentsDlg_H
