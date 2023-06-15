#ifndef UDELUNDO_H
#define UDELUNDO_H
#include <QUndoCommand>

class UWidgetScene;
class UDelUndo : public QUndoCommand
{
public:
    UDelUndo(UWidgetScene * scene,QList<QWidget*> items);
    virtual ~UDelUndo();

    //redo
    void redo();

    //undo
    void undo();

private:
    UWidgetScene * m_scene = nullptr;
    QList<QWidget*> m_itemlist;
    bool m_isPushRedoCMD = true;
};

#endif // UDELUNDO_H
