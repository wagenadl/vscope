
// guiarraypage.h

#ifndef GUIARRAYPAGE_H

#define GUIARRAYPAGE_H

#include <gui/guipage.h>

class guiArrayPage: public guiPage {
  Q_OBJECT;
public:
  guiArrayPage(class QWidget *parent,
	  class ParamTree *ptree,
	  QString id,
	  class guiRoot *master,
	  class QRect const &geom);
  virtual void connectToParent(EasyXML doc);
  virtual ~guiArrayPage();
  virtual QString getCurrentElement() const;
  virtual void reconnect();
public slots:
  virtual void open();
  virtual void open(QString elt);
  /*:F open
   *:D This version opens a specific sub-page.
  */
protected:
  void retree();
private:
  QString currentElement;
  /*:V currentElement
   *:D This is the ID of the currently open tab.
  */

};

#endif
