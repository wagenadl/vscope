// base/progressdialog.cpp - This file is part of VScope.
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

// progressdialog.cpp

#include "progressdialog.h"
#include <QCoreApplication>

ProgressDialog::ProgressDialog(QString text):
  QProgressDialog(text, "Cancel", 0, 100, root()) {
  setWindowModality(Qt::WindowModal);
  setWindowTitle("VScope - Progress");
  setLabelText(text);
  setMinimumDuration(100);
  setCancelButton(0);
  starts << 1;
  ends << 100;
  texts << text;
  progress(1);
  show();
}

void ProgressDialog::setRoot(QWidget *r) {
  root() = r;
}

QWidget *&ProgressDialog::root() {
  static QWidget *r = 0;
  return r;
}

void ProgressDialog::push(float percentile, QString txt) {
  float now = ends.last() - starts.last();
  starts << current;
  ends << current + now*percentile/100;
  if (txt.isEmpty()) {
    texts << texts.last();
  } else {
    texts << txt;
    setLabelText(txt);
  }
}

void ProgressDialog::pop() {
  progress(100);
  if (ends.size()>1) {
    ends.removeLast();
    starts.removeLast();
    texts.removeLast();
  }
  setLabelText(texts.last());
}

void ProgressDialog::progress(float c) {
  current = starts.last() + c/100*(ends.last()-starts.last());
  setValue(current);
}
