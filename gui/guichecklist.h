// guichecklist.h

#ifndef GUICHECKLIST_H

#define GUICHECKLIST_H

#include <gui/guipage.h>

class guiChecklist: public guiPage {
  Q_OBJECT;
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
  virtual guiItem *createItem(QString id);
  virtual void prepForOpening();
};

#endif
