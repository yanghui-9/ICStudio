#ifndef URectUndo_H
#define URectUndo_H
#include <QUndoCommand>
#include <QRect>

class URectUndo: public QUndoCommand
{
public:
    URectUndo(QRect begin, QRect end, QWidget* item);
    virtual ~URectUndo();

    //redo
    void redo();

    //undo
    void undo();
private:
    QRect m_begin;
    QRect m_end;
    QWidget* m_item;
    bool m_isPushRedoCMD = true;
};

#endif // URectUndo_H
