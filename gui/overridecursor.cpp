// overridecursor.cpp

#include "overridecursor.h"
#include <QApplication>

OverrideCursor::OverrideCursor() {
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  actv = true;
}

OverrideCursor::OverrideCursor(QCursor const &c) {
  QApplication::setOverrideCursor(c);
}

OverrideCursor::~OverrideCursor() {
  restore();
}

void OverrideCursor::restore() {
  if (actv)
    QApplication::restoreOverrideCursor();
  actv = false;
}
