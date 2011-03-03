// dbg.cpp

#include <base/dbg.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <QTime>
#include <base/exception.h>
#include <QRectF>
#include <QLineF>

Dbg::Dbg() throw() {
  try {
    txt = "[" + QTime::currentTime().toString("hhmmss.zzz") + "] ";
    setString(&txt);
  } catch(...) {
    ;
  }
}

Dbg::~Dbg() throw () {
  try {
    if (!txt.endsWith("\n"))
      txt += "\n";
    
    fprintf(stderr,"%s",qPrintable(txt));
    if (!dbgfile)
      dbgfile = new DbgFile();
    *dbgfile << txt;
  } catch (...) {
    ;
  }
}

Warning::Warning() throw() {
  try {
    txt += "WARNING: ";
  } catch (...) {
    ;
  }
}

// ----------------------------------------------------------------------
void warn(char const *fmt, ...) throw() {
  va_list ap;
  va_start(ap,fmt);
  char txt[10240];
  vsnprintf(txt,10240,fmt,ap);
  Warning() << txt;
  va_end(ap);
}

void dbg(char const *fmt, ...) throw() {
  va_list ap;
  va_start(ap,fmt);
  char txt[10240];
  vsnprintf(txt,10240,fmt,ap);
  Dbg() << txt;
  va_end(ap);
}

//----------------------------------------------------------------------
DbgFile *dbgfile = 0;

DbgFile::DbgFile() {
  ts=0;
}

DbgFile::~DbgFile() {
  if (ts)
    delete ts;
}

void DbgFile::setDir(QString const &d) {
  dir = d;
  if (ts)
    delete ts;
  ts = 0;
  f.close();
}

DbgFile &DbgFile::operator<<(QString const &str) {
  if (!ts) {
    f.setFileName(dir + "/debug.txt");
    if (f.exists()) {
      int n=0;
      while (f.exists()) {
	n++;
	f.setFileName(dir + QString("/debug-%1.txt").arg(n));
      }
    }
    if (f.open(QIODevice::WriteOnly | QIODevice::Append)) {
      ts = new QTextStream(&f);
      foreach (QString s, backlog) {
	*ts << s;
      }
      backlog.clear();
    } else {
      backlog.append(str);
    }
  }
  if (ts) {
    *ts << str;
    ts->flush();
  }
  return *this;
}

//Dbg &Dbg::operator<<(QString const &x) {
//  *(QTextStream*)this << x;
//  return *this;
//}
//
//Dbg &Dbg::operator<<(int x) {
//  *(QTextStream*)this << x;
//  return *this;
//}
//
//Dbg &Dbg::operator<<(unsigned int x) {
//  *(QTextStream*)this << x;
//  return *this;
//}
//
//Dbg &Dbg::operator<<(void const *x) {
//  *(QTextStream*)this << x;
//  return *this;
//}
//
//Dbg &Dbg::operator<<(char const *x) {
//  *(QTextStream*)this << x;
//  return *this;
//}
//
//Dbg &Dbg::operator<<(double x) {
//  *(QTextStream*)this << x;
//  return *this;
//}

Dbg &Dbg::operator<<(class QPoint const &x) {
  return *this << QString("(%1,%2)")
    .arg(x.x()).arg(x.y());
}

Dbg &Dbg::operator<<(class QPointF const &x) {
  return *this << QString("(%1,%2)")
    .arg(x.x()).arg(x.y());
}

Dbg &Dbg::operator<<(class QSize const &x) {
  return *this << QString("(%1x%2)")
    .arg(x.width()).arg(x.height());
}

Dbg &Dbg::operator<<(class QSizeF const &x) {
  return *this << QString("(%1x%2)")
    .arg(x.width()).arg(x.height());
}

Dbg &Dbg::operator<<(class QRect const &x) {
  return *this << QString("(%1x%2+%3+%4)")
    .arg(x.width()).arg(x.height())
    .arg(x.left()).arg(x.top());
}

Dbg &Dbg::operator<<(class QRectF const &x) {
  return *this << QString("(%1x%2+%3+%4)")
    .arg(x.width()).arg(x.height())
    .arg(x.left()).arg(x.top());
}

Dbg &Dbg::operator<<(class QLine const &x) {
  return *this << x.p1() << "-" << x.p2();
}

Dbg &Dbg::operator<<(class QLineF const &x) {
  return *this << x.p1() << "-" << x.p2();
}
