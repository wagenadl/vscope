// guiItem.h

#ifndef GUIITEM_H

#define GUIITEM_H

#include <gui/guibutton.h>

class guiItem: public guiButton {
  Q_OBJECT;
public:
  guiItem(class guiPage *parent, QString id, class guiRoot *master=0);
  virtual ~guiItem();
  virtual void setup(EasyXML doc);
protected:
  virtual void connectUp(EasyXML)=0;
  virtual void stylize(EasyXML);
};

#endif
