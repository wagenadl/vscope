// timebutton.h

#ifndef TIMEBUTTON_H

#define TIMEBUTTON_H

#include <QString>
#include <QTime>
#include <QBasicTimer>
#include <QObject>

class TimeButton: public QObject {
  Q_OBJECT;
public:
  enum Style { None, Wall, Up, Down };
public:
  TimeButton(QObject *parent, class xmlButton *button);
  virtual ~TimeButton();
public slots:
  void reset();
  void startCountUp(bool onlyifstopped=false);
  void startCountDown(double time_ms);
  void showWallTime();
  void show();
  void hide();
  void setFormat(QString f);
protected:
  virtual void timerEvent(class QTimerEvent *);
private:
  void relabel();
private:
  Style style;
  QTime time;
  QBasicTimer timer;
  QString caption;
  class xmlButton *button;
};

#endif
