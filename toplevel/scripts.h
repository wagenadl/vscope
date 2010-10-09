// scripts.h

#ifndef SCRIPTS_H

#define SCRIPTS_H

#include <QString>
#include <QWidget>
#include <xml/script.h>

class Scripts: public QWidget {
  Q_OBJECT;
public:
  Scripts(QWidget *parent);
  virtual ~Scripts();
public slots:
  void prepareLoad();
  void prepareSave();
  void resave();
  void check();
  void run();
  void stop();
  void setRunning(bool run);
  void doLoad(QString);
  void doSave(QString);
  void setDir(QString);
  void setName(QString);
public:
  bool status();
  void timerEvent(QTimerEvent *);
private:
  void runSome();
private:
  class QTextEdit *editor;
  class QLabel *statusLabel;
  class QLabel *nameLabel;
  class FileDlgKey *loaddlg;
  class QFrame *loadframe;
  class FileDlgKey *savedlg;
  class QFrame *saveframe;
  QString lastdir;
  QString name;
private:
  bool active;
  Script script;
  QList<Script::Command>::const_iterator it;
  double ival_ms;
};

#endif
