// guichecklist.h

#ifndef GUICHECKLIST_H

#define GUICHECKLIST_H

#include <gui/guipage.h>

class guiChecklist: public guiPage {
public:
  guiChecklist(class QWidget *parent,
	  class ParamTree *ptree,
	  QString id,
	  class guiRoot *master,
	  class QRect const &geom);
  virtual void setup(QDomElement doc);
  virtual ~guiChecklist();
protected:
  virtual Button::VisualType visualTypeForParentButton() const;
  virtual class guiButton *addItem(PageBuildGeom &g, QDomElement elt);
};

#endif
