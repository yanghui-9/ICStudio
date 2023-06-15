#ifndef UADDUNDO_H
#define UADDUNDO_H
#include <QUndoCommand>

class UWidgetScene;
class UAddUndo : public QUndoCommand
{
public:
    UAddUndo(UWidgetScene * scene,QList<QWidget*> items);
    virtual ~UAddUndo();

    //redo
    void redo();

    //undo
    void undo();
private:
    UWidgetScene * m_scene = nullptr;
    QList<QWidget*> m_itemlist;
    bool m_isPushRedoCMD = true;
};

#endif // UADDUNDO_H
