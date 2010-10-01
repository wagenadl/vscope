// blackout.cpp

#include "blackout.h"

Blackout::Blackout(QWidget *parent): QLabel(parent) {
  setText("Acquiring...");
  setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
  QPalette p=palette();
  p.setColor(QPalette::Window,QColor("black"));
  p.setColor(QPalette::WindowText,QColor("red"));
  setPalette(p);
  setAutoFillBackground(true);
  setGeometry(parent ? parent->rect() : QRect(0,0,1024,768));
}

Blackout::~Blackout() {
}
