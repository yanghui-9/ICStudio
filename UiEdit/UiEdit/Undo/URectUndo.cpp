#include "URectUndo.h"
#include "../UIInputPolicy/RectEdit_UIInput.h"

URectUndo::URectUndo(QRect begin, QRect end, QWidget * item):
    m_begin(begin),m_end(end),m_item(item)
{
}

URectUndo::~URectUndo()
{
}

void URectUndo::redo()
{
    if(m_isPushRedoCMD)
    {
        m_isPushRedoCMD = false;
        return;
    }
    if(m_item)
    {
        m_item->setGeometry(m_end.x(),
                            m_end.y(),
                            m_end.width(),
                            m_end.height());
        RectEdit_UIInput::DealCombineItem(m_item);
    }
}

void URectUndo::undo()
{
    if(m_item)
    {
        m_item->setGeometry(m_end.x(),
                            m_end.y(),
                            m_begin.width(),
                            m_begin.height());
        RectEdit_UIInput::DealCombineItem(m_item);
    }
}
