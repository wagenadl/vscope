// overridecursor.cpp

#include "overridecursor.h"
#include <QApplication>

OverrideCursor::OverrideCursor() {
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

OverrideCursor::OverrideCursor(QCursor const &c) {
  QApplication::setOverrideCursor(c);
}

OverrideCursor::~OverrideCursor() {
  QApplication::restoreOverrideCursor();
}
