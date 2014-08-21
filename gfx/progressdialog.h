// progressdialog.h

#ifndef PROGRESSDIALOG_H

#define PROGRESSDIALOG_H

#include <QProgressDialog>

class ProgressDialog: private QProgressDialog {
public:
  ProgressDialog(QString text);
public:
  static void setRoot(QWidget *);
private:
  static QWidget *&root();
};

#endif
