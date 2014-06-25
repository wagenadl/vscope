// controlpanel.cpp

#include "controlpanel.h"

ControlPanel::ControlPanel(ParamTree *ptree, EasyXML gd, QWidget *parent):
  AbstractPage(parent), guidef(gd) {
  resize(guidef.size("width", "height"));
  rt = new ControlPage(ptree, guidef.firstChild("page"), this);
}

virtual ControlPanel::~ControlPanel() {
}


QRect ControlPanel::childPageRect() const {
  return QRect(QPoint(0,0), size());
}
