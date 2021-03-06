// base/progressdialog.h - This file is part of VScope.
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

// progressdialog.h

#ifndef PROGRESSDIALOG_H

#define PROGRESSDIALOG_H

#include <QProgressDialog>

class ProgressDialog: public QProgressDialog {
public:
  ProgressDialog(QString text);
  void push(float percentage, QString text="");
  void progress(float percentage);
  void pop();
public:
  static void setRoot(QWidget *);
private:
  static QWidget *&root();
private:
  QList<float> starts;
  QList<float> ends;
  QList<QString> texts;
  float current;
};

#endif
