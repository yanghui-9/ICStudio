#ifndef UMOVEUNDO_H
#define UMOVEUNDO_H
#include <QUndoCommand>
#include <QPoint>

class UMoveUndo: public QUndoCommand
{
public:
    UMoveUndo(QPoint begin,QPoint end,QList<QWidget*> itemlist);
    virtual ~UMoveUndo();

    //redo
    void redo();

    //undo
    void undo();

private:
    QPoint m_begin;
    QPoint m_end;
    QList<QWidget*> m_itemlist;
    bool m_isPushRedoCMD = true;
};

#endif // UMOVEUNDO_H
