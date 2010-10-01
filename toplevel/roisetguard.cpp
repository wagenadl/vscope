// roisetguard.cpp

#include "roisetguard.h"
#include <base/xml.h>
#include <QDomElement>

ROISetGuard::ROISetGuard(QObject *parent): QObject(parent) {
  fn = "";
}

ROISetGuard::~ROISetGuard() {
}

void ROISetGuard::setfn(QString fn1) {
  fn=fn1;
}

void ROISetGuard::save() {
  if (!fn.isEmpty()) 
    ROISet::save(fn);
}

void ROISetGuard::load(QString fn1) {
  fn=fn1;
  ROISet::load(fn);
}
