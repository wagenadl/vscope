// scripts.h

#ifndef SCRIPTS_H

#define SCRIPTS_H

#include <QString>
#include <QWidget>
#include <QTimer>

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
private slots:
  void doLoad(QString);
  void doSave(QString);
  void setDir(QString);
  void setName(QString);
  void timeout();
  void changed();
public:
  bool status();
private:
  void runSome();
private:
  class TextEdit *editor;
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
  QTimer timer;
  int lineno;
  int loopno;
};

#endif
