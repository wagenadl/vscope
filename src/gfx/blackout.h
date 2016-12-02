// blackout.h

#ifndef BLACKOUT_H

#define BLACKOUT_H

#include <QLabel>
#include <QTime>

class Blackout: public QLabel {
  /*:C Blackout
   *:D Blacks out the entire GUI.
       For use during acquisition.
  */
  Q_OBJECT;
public:
  Blackout(QWidget *parent);
  virtual ~Blackout();
  virtual void setText(QString);
  void countUp();
private slots:
  void timeout();
private:
  QString txt;
  QTime t0;
  class QTimer *timer;
};

#endif
