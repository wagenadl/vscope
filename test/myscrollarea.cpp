// myscrollarea.cpp

#include "myscrollarea.h"
#include <base/dbg.h>

MyScrollArea::MyScrollArea(QWidget *parent): QScrollArea(parent) {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}


void MyScrollArea::resizeEvent(class QResizeEvent *e) {
  QScrollArea::resizeEvent(e);
  if (widget()) {
    int w = widget()->geometry().width();
    ensureVisible(w-1,1,1,1);
    dbg("reisze w=%i",w);
  }
}
