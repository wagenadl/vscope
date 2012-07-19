// guitabbedpage.h

#ifndef GUITABBEDPAGE_H

#define GUITABBEDPAGE_H

#include <guipage.h>

class guiTabbedPage: public guiPage {
public:
  guiTabbedPage(class QWidget *parent,
	  class ParamTree *ptree,
	  QString id,
	  class guiRoot *master,
	  class QRect const &geom);
  virtual void connectToParent(QDomElement doc);
  virtual ~guiTabbedPage();
};

#endif
