// gui.cpp

#include "gui.h"

GUI::GUI(ParamTree *ptree, EasyXML guidef) {
  geom = EasyXML(guidef, "geom");
  resize(geom.size("width", "height"));
  
  cp = new ControlPanel(ptree, guidef, this);
  cp->move(geom.point("controlx0", "controly0"));

  lv = new Viewer(this);
  lv->move(0, 0);

  rv = new Viewer(this);
  rv->move(geom.integer("width") - geom.integer("viewerw"), 0);
}

GUI::~GUI() {
}

void GUI::addViewable(QString id, QWidget *w) {
  if (viewbl.contains(id)) {
    if (viewbl[id] == w)
      return; // already contained
    else if (viewbl[id])
      delete viewbl[id];
  }
  viewbl[id] = w;
}

void GUI::removeViewable(QString id) {
  viewbl.remove(id);
}

void GUI::removeViewable(QWidget *w) {
  foreach (QString id, viewbl.keys())
    if (viewbl[id]==w)
      viewbl.remove(id);
}

QWidget *GUI::viewable(QString id) const {
  if (viewbl.contains(id))
    return viewbl[id];
  else
    return 0;
}

QStringList GUI::viewables() const {
  return viewbl.keys();
}
