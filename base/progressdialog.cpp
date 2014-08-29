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
  setValue(1);
  show();
}

void ProgressDialog::setRoot(QWidget *r) {
  root() = r;
}

QWidget *&ProgressDialog::root() {
  static QWidget *r = 0;
  return r;
}
