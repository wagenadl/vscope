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
  virtual class guiButton *addItem(PageBuildGeom &g, QDomElement elt);
public slots:
  virtual void updateAuto();
  virtual void overrideAuto(QStringList ids);
  void setNonExclusive();
protected:
  virtual VISUALTYPE visualTypeForParentButton() const;
  virtual void connectToParent(QDomElement doc);
  virtual guiItem *createItem(QString id); // simply constructs a guiMenuItem
  virtual void prepForOpening();
  virtual void addAuto(PageBuildGeom &g, QDomElement doc);
  virtual bool mayResize();
protected:
  class AutoItems *autoItems;
  class RadioGroup *itemgroup;
  bool overridden;
};

#endif
