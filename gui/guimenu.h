// guimenu.h

#ifndef GUIMENU_H

#define GUIMENU_H

#include <gui/guipage.h>

class guiMenu: public guiPage {
  Q_OBJECT;
public:
  guiMenu(class QWidget *parent,
	  class ParamTree *ptree,
	  QString id,
	  class guiRoot *master,
	  class QRect const &geom);
  virtual ~guiMenu();
public slots:
  void updateAuto();
protected:
  virtual VISUALTYPE visualTypeForParentButton() const;
  virtual void connectToParent(QDomElement doc);
  virtual guiItem *createItem(QString id); // simply constructs a guiMenuItem
  virtual void prepForOpening();
  virtual void addAuto(PageBuildGeom &g, QDomElement doc);
  virtual bool mayResize();
protected:
  class AutoItems *autoItems;
};

#endif
