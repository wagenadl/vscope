// guiChecklistitem.h

#ifndef GUICHECKLISTITEM_H

#define GUICHECKLISTITEM_H

#include <gui/guiitem.h>

class guiChecklistItem: public guiItem {
public:
  guiChecklistItem(QWidget *parent, QString id, class guiRoot *master=0);
  virtual ~guiChecklistItem();
protected:
  virtual void connectUp(QDomElement);
};

#endif
