// blackout.cpp

#include "blackout.h"
#include <QTimer>

Blackout::Blackout(QWidget *parent): QLabel(parent) {
  timer = new QTimer(this);
  setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
  QPalette p=palette();
  p.setColor(QPalette::Window,QColor("black"));
  p.setColor(QPalette::WindowText,QColor("red"));
  setPalette(p);
  setAutoFillBackground(true);
  setGeometry(parent ? parent->rect() : QRect(0,0,1024,768));
  connect(timer, SIGNAL(timeout()), SLOT(timeout()));
}

Blackout::~Blackout() {
}

void Blackout::setText(QString s) {
  txt = s;
  timer->stop();
  QLabel::setText(s);
}

void Blackout::countUp() {
  t0.start();
  timer->start(0.1);
}

void Blackout::timeout() {
  QLabel::setText(txt + QString(" %1").arg(t0.elapsed()/1e3, 0, 'f', 3));
}
