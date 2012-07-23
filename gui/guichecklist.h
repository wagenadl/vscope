// guichecklist.h

#ifndef GUICHECKLIST_H

#define GUICHECKLIST_H

#include <gui/guimenu.h>

class guiChecklist: public guiMenu {
  Q_OBJECT;
public:
  guiChecklist(class QWidget *parent,
	  class ParamTree *ptree,
	  QString id,
	  class guiRoot *master,
	  class QRect const &geom);
  virtual ~guiChecklist();
protected:
  virtual guiItem *createItem(QString id);
  virtual void prepForOpening();
};

#endif
