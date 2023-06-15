#ifndef ITEMSFACTORY_H
#define ITEMSFACTORY_H
#include <QObject>
#include <QGraphicsItem>
#include <QList>
#include <QLibrary>
#include <QAction>
#include <functional>

class UTabWidget;
class itemsFactory : public QObject
{
    Q_OBJECT
public:
    typedef QWidget* (*createItem)(QWidget *);
    typedef bool (*item_object_create)(const std::string &,QWidget *,QJsonObject *);
    typedef bool (*item_object_save)(const std::string &,QWidget *,QJsonObject *);
    typedef void (*getItemName)(std::string &);
    typedef QAction* (*getItemAction)(const std::string &);

    virtual ~itemsFactory();

    //初始化
    void init();
    //创建控件
    QWidget * createItemFromName(QString &name, QWidget *parent = nullptr, bool bNoCustom = false);
    void createItemObjFromName(const QString &className, QWidget *item, QJsonObject *object = nullptr);
    void saveItemObjFromName(const QString &className, QWidget *item, QJsonObject *object = nullptr);

    //获取控件列表及其名称
    int getItemProList(QList<QString> &nameList, QList<QAction*> &acList);

    //设置主视图指针
    void SetView(UTabWidget * view){
        m_view = view;
    }

    static itemsFactory* GetInstance();
    static void ReleaseInstace();

protected:
    explicit itemsFactory(QObject *parent = nullptr);

    void InitCombineItems();

    void LoadCombineItem(const QString &fileName, const QString &filePath);

protected slots:
    void dealActionSlot();

signals:
    void ItemPress(const QString &);

private:
    static itemsFactory *instance;

    QList<createItem> m_list0;
    QList<item_object_create> m_list1;
    QList<item_object_save> m_list2;
    QList<QString> m_listName;
    QList<QLibrary*> m_listLib;
    QList<int> m_itemFlags;
    QList<QAction*> m_itemsAction;

    QMap<QString,QJsonObject> m_combineItems;

    UTabWidget * m_view = nullptr;
};

#endif // ITEMSFACTORY_H
