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
  if (!this)
    return;
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
  if (!this)
    return;
  progress(100);
  if (ends.size()>1) {
    ends.removeLast();
    starts.removeLast();
    texts.removeLast();
  }
  setLabelText(texts.last());
}

void ProgressDialog::progress(float c) {
  if (!this)
    return;
  current = starts.last() + c/100*(ends.last()-starts.last());
  setValue(current);
}
