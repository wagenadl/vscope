// gui.h

#ifndef GUI_H

#define GUI_H

#include <QWidget>
#include "guigeom.h"
#include "EasyXML.h"

class GUI: public QWidget {
  Q_OBJECT;
public:
  GUI(ParamTree *ptree, EasyXML guidef);
  virtual ~GUI();
  class ControlPanel *controlPanel() { return cp; }
  class Viewer *leftViewer() { return lv; }
  class Viewer *rightViewer() { return rv; }
  void addViewable(QString id, QWidget *); // we become responsible
  void removeViewable(QString id); // we give up responsibility
  void removeViewable(QWidget *);
  QWidget *viewable(QString id) const; // 0 if not found
  QStringList viewables() const;
private:
  EasyXML geom;
  class ControlPanel *cp;
  class Viewer *lv;
  class Viewer *rv;
  QMap<QString, QWidget *> viewbl;
};

#endif
