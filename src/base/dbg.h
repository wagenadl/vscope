// dbg.h

#ifndef DBG_H
#define DBG_H

#include <QDebug>
#include <QObject>
#include <QFile>

extern void dbg(char const *fmt,...) throw();
extern void warn(char const *fmt,...) throw();

class DbgBase: public QDebug {
public:
  DbgBase(QString *string);
  virtual ~DbgBase();
protected:
  QString *string;
};

class Dbg: public DbgBase {
public:
  Dbg(QObject const *src=0);
  virtual ~Dbg();
};

class Warning: public Dbg {
public:
  Warning();
  virtual ~Warning() throw();
  static void enableGUIWarnings();
  static void disableGUIWarnings();
private:
  QString t0;
  static bool guiwarn_enabled;
};

class DbgFile {
public:
  DbgFile();
  virtual ~DbgFile();
  void setDir(QString const &dir);
  DbgFile &operator<<(QString const &);
private:
  QString dir;
  QFile f;
  QTextStream *ts;
  QStringList backlog;
private: // not instantiated
  DbgFile(DbgFile const &);
  void operator=(DbgFile const &);
};

extern DbgFile *dbgfile;

#endif
