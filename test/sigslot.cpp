// sigslot.cpp

#include <stdio.h>
#include <QApplication>

#include "ss_source.h"
#include "ss_recv.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  Source s;
  Recv r;
  QObject::connect(&s,SIGNAL(intsig(int,int)),&r,SLOT(recv()));
  QObject::connect(&s,SIGNAL(intsig(int,int)),&r,SLOT(recvint(int)));
  QObject::connect(&s,SIGNAL(intsig(int,int)),&r,SLOT(recvintint(int,int)));
  QObject::connect(&s,SIGNAL(intsig(int,int)),&r,SLOT(recvintint(int,int)));
  QObject::connect(&s,SIGNAL(intsig(int,int)),&r,SLOT(recvstring(QString)));
  QObject::connect(&s,SIGNAL(intsig(int,int)),&r,SLOT(recvintstring(int,QString)));

  s.makeint(17,23);
  printf("starting app\n");
  return app.exec();
}

  
