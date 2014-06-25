// guivalueitem.h

#ifndef GUIVALUEITEM_H

#define GUIVALUEITEM_H

#include <gui/guibutton.h>

class guiValueItem: public guiButton {
  Q_OBJECT;
public:
  guiValueItem(class guiPage *parent, QString id, class guiRoot *master=0);
  virtual ~guiValueItem();
  virtual void setup(EasyXML doc);
  bool allowsMultiple() const { return multiok; }
  void setAllowMultiple();
private:
  bool multiok;
};

#endif
