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
  //Dbg &operator<<(char const *);
  //Dbg &operator<<(void const *);
  //Dbg &operator<<(QString const &);
  //Dbg &operator<<(unsigned int);
  //Dbg &operator<<(int);
  //Dbg &operator<<(double);
  Dbg &operator<<(class QPoint const &);
  Dbg &operator<<(class QSize const &);
  Dbg &operator<<(class QRect const &);
  Dbg &operator<<(class QLine const &);
  Dbg &operator<<(class QPointF const &);
  Dbg &operator<<(class QSizeF const &);
  Dbg &operator<<(class QRectF const &);
  Dbg &operator<<(class QLineF const &);
  Dbg &operator<<(class QObject const *);
  template <class X> Dbg &operator<<(X const &x) {
    *(QTextStream*)this << x; return *this;
  }
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

extern DbgFile *dbgfile;

#endif
