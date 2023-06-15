#ifndef BACKGROUNDSETTINGSW_H
#define BACKGROUNDSETTINGSW_H

#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
class item_base;
class BackgroundSettingsW : public QGroupBox
{
    Q_OBJECT
public:
    BackgroundSettingsW(item_base *item, QWidget *parent = nullptr);

    bool IsImg(){
        return picRadio->isChecked();
    }
    QColor GetColor(){
        return m_bgColor;
    }
    QString GetImg(){
        return picBtn->text();
    }

    void IsImgEnable(bool flag){
        colorRadio->setChecked(!flag);
        picRadio->setChecked(flag);

    }
    void SetColor(QColor color){
       m_bgColor = color;
       //按钮设置背景色
       colorBtn->setStyleSheet(QString("background-color:#%1%2%3")
                               .arg(color.red(),2,16,QChar('0'))
                               .arg(color.green(),2,16,QChar('0'))
                               .arg(color.blue(),2,16,QChar('0')));
    }
    void SetImg(const QString &name){
        picBtn->setText(name);
    }

protected slots:
    //颜色选择
    void colorSelectSlot();
    //图片选择
    void imgSelectSlot();
    //背景类型切换
    void bgTypeChangeSlot();

private:
    QRadioButton * colorRadio;
    QPushButton * colorBtn;
    QRadioButton * picRadio;
    QPushButton * picBtn;

    QColor m_bgColor;

    item_base* m_item;
};

#endif // BACKGROUNDSETTINGSW_H
