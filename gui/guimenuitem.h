// guiMenuitem.h

#ifndef GUIMENUITEM_H

#define GUIMENUITEM_H

#include <gui/guiitem.h>

class guiMenuItem: public guiItem {
  Q_OBJECT;
public:
  guiMenuItem(class guiPage *parent, QString id, class guiRoot *master=0);
  virtual ~guiMenuItem();
protected:
  virtual void connectUp(QDomElement);
};

#endif
