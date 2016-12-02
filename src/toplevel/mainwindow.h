// mainwindow.h

#ifndef MAINWINDOW_H

#define MAINWINDOW_H

#include <QWidget>

class MainWindow: public QWidget {
  Q_OBJECT;
public:
  MainWindow(class QApplication &app);
  virtual ~MainWindow();
  int basey() const { return basey_; }
private:
  int basey_;
};

#endif
