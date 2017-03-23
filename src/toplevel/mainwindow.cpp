// toplevel/mainwindow.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

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
