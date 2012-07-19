// guimenu.h

#ifndef GUIMENU_H

#define GUIMENU_H

#include <guipage.h>

class guiMenu: public guiPage {
public:
  guiMenu(class QWidget *parent,
	  class ParamTree *ptree,
	  QString id,
	  class guiRoot *master,
	  class QRect const &geom);
  virtual void setup(QDomElement doc);
  virtual ~guiMenu();
protected:
  virtual Button::VisualType visualTypeForParentButton() const;
  virtual void connectToParent(QDomElement doc);
  virtual guiItem *createItem(QString id); // simply constructs a guiMenuItem
};

#endif
