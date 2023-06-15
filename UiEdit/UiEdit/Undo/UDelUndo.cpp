#include "UDelUndo.h"
#include "../UWidgetScene.h"

UDelUndo::UDelUndo(UWidgetScene * scene,QList<QWidget*> items):m_scene(scene),m_itemlist(items)
{
}

UDelUndo::~UDelUndo()
{
  if(m_itemlist.size() > 0)
  {
      foreach (auto item, m_itemlist) {
          if(!item->parent())
          {//控件当前不属于任何画面
              item->deleteLater();
          }
      }
      m_itemlist.clear();
  }
}

void UDelUndo::redo()
{
    if(m_isPushRedoCMD)
    {
        m_isPushRedoCMD = false;
        return;
    }
    if(m_scene)
    {
        m_scene->removeItemObject(m_itemlist,true);
    }
}

void UDelUndo::undo()
{
    if(m_scene)
    {
        foreach (auto item, m_itemlist) {
            item->setParent(m_scene);
            item->setVisible(true);
            m_scene->addItem(item);
        }
    }
}
