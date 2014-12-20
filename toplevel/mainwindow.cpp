// mainwindow.cpp

#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <base/dbg.h>

MainWindow::MainWindow(QApplication &app) {
  setWindowTitle("VScope");
  int deskw = app.desktop()->width();
  int deskh = app.desktop()->height();
  int winh=768;
  if (deskw>1050 && deskh>=800) {
    setGeometry(deskw/2-512,deskh/2-384,1024,winh);
  } else {
    winh=deskh;
    setWindowState(Qt::WindowFullScreen);
  }
  basey_ = winh-256;
}

MainWindow::~MainWindow() {
}
