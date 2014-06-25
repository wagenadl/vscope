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
  virtual class guiButton *addValueItem(PageBuildGeom &g, EasyXML elt);
  virtual class guiButton *addValueItems(PageBuildGeom &g, EasyXML elt);
public slots:
  virtual void updateAuto();
  void setNonExclusive();
protected:
  virtual VISUALTYPE visualTypeForParentButton() const;
  virtual void connectToParent(EasyXML doc);
  virtual guiItem *createItem(QString id); // simply constructs a guiMenuItem
  virtual void prepare();
  virtual void addAuto(PageBuildGeom &g, EasyXML doc);
  virtual bool mayResize();
protected:
  class AutoItems *autoItems;
  class RadioGroup *itemgroup;
};

#endif
