// progressdialog.cpp

#include "progressdialog.h"

ProgressDialog::ProgressDialog(QString text):
  QProgressDialog(text, "Cancel", 0, 100, root()) {
  setWindowModality(Qt::WindowModal);
  setLabelText(text);
  setMinimumDuration(100);
  setCancelButton(0);
  setValue(1);
}

void ProgressDialog::setRoot(QWidget *r) {
  root() = r;
}

QWidget *&ProgressDialog::root() {
  static QWidget *r = 0;
  return r;
}
