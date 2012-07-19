// guiItem.h

#ifndef GUIITEM_H

#define GUIITEM_H

#include <gui/guibutton.h>

class guiItem: public guiButton {
public:
  guiItem(QWidget *parent, QString id, class guiRoot *master=0);
  virtual void setup(QDomElement doc);
  virtual ~guiItem();
protected:
  virtual void connectUp(QDomElement);
  virtual void stylize(QDomElement);
};

#endif
