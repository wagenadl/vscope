// guitabbedpage.h

#ifndef GUITABBEDPAGE_H

#define GUITABBEDPAGE_H

#include <guipage.h>

class guiTabbedPage: public guiPage {
  Q_OBJECT;
public:
  guiTabbedPage(class QWidget *parent,
	  class ParamTree *ptree,
	  QString id,
	  class guiRoot *master,
	  class QRect const &geom);
  virtual void connectToParent(QDomElement doc);
  virtual ~guiTabbedPage();
  virtual QString getCurrentElement() const;
  virtual void reconnect();
public slots:
  virtual void open(QString elt);
  /*:F open
   *:D This version opens a specific sub-page.
  */
private:
  QString currentElement;
  /*:V currentElement
   *:D This is the ID of the currently open tab.
  */

};

#endif
