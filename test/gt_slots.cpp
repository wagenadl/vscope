// gt_slots.cpp

#include "gt_slots.h"
#include <QApplication>

gt_slots::gt_slots() {
}

gt_slots::~gt_slots() {
}

void gt_slots::clicked(QString p) {
  printf("clicked %s\n",qPrintable(p));
  if (p=="quit")
    QApplication::quit();
}

void gt_slots::selected(QString p) {
  printf("selected %s\n",qPrintable(p));
}

void gt_slots::deselected(QString p) {
  printf("deselected %s\n",qPrintable(p));
}

void gt_slots::paramchanged(QString p, QString val) {
  printf("param changed %s -> %s\n",qPrintable(p),qPrintable(val));
}

void gt_slots::customchanged(QString p, int cno, QString val) {
  printf("customchanged %s (%i) -> %s\n",qPrintable(p),cno,qPrintable(val));
}

