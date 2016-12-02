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
