// dbg.h

#ifndef DBG_H
#define DBG_H

#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QFile>

extern void dbg(char const *fmt,...) throw();
extern void warn(char const *fmt,...) throw();


class Dbg: public QTextStream {
public:
  Dbg() throw();
  virtual ~Dbg() throw();
protected:
  QString txt;
};

class Warning: public Dbg {
public:
  Warning() throw();
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
};

extern DbgFile dbgfile;

#endif
