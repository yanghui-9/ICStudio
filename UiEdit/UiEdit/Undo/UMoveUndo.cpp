#include "UMoveUndo.h"
#include <QWidget>

UMoveUndo::UMoveUndo(QPoint begin, QPoint end, QList<QWidget *> itemlist):
    m_begin(begin),m_end(end),m_itemlist(itemlist)
{
}

UMoveUndo::~UMoveUndo()
{
}

void UMoveUndo::redo()
{
    if(m_isPushRedoCMD)
    {
        m_isPushRedoCMD = false;
        return;
    }
    int xd = m_begin.x() - m_end.x();
    int yd = m_begin.y() - m_end.y();
    foreach (auto item, m_itemlist) {
        item->move(item->pos().x()-xd,item->pos().y()-yd);
    }
}

void UMoveUndo::undo()
{
    int xd = m_begin.x() - m_end.x();
    int yd = m_begin.y() - m_end.y();
    foreach (auto item, m_itemlist) {
        item->move(item->pos().x()+xd,item->pos().y()+yd);
    }
}
