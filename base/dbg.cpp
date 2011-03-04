// dbg.cpp

#include <base/dbg.h>
#include <base/object.h>

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

Dbg &Dbg::operator<<(QPoint const &x) {
  return *this << QString("(%1,%2)")
    .arg(x.x()).arg(x.y());
}

Dbg &Dbg::operator<<(QPointF const &x) {
  return *this << QString("(%1,%2)")
    .arg(x.x()).arg(x.y());
}

Dbg &Dbg::operator<<(QSize const &x) {
  return *this << QString("(%1x%2)")
    .arg(x.width()).arg(x.height());
}

Dbg &Dbg::operator<<(QSizeF const &x) {
  return *this << QString("(%1x%2)")
    .arg(x.width()).arg(x.height());
}

Dbg &Dbg::operator<<(QRect const &x) {
  return *this << QString("(%1x%2+%3+%4)")
    .arg(x.width()).arg(x.height())
    .arg(x.left()).arg(x.top());
}

Dbg &Dbg::operator<<(QRectF const &x) {
  return *this << QString("(%1x%2+%3+%4)")
    .arg(x.width()).arg(x.height())
    .arg(x.left()).arg(x.top());
}

Dbg &Dbg::operator<<(QLine const &x) {
  return *this << x.p1() << "-" << x.p2();
}

Dbg &Dbg::operator<<(QLineF const &x) {
  return *this << x.p1() << "-" << x.p2();
}

Dbg &Dbg::operator<<(QObject const *x) {
  return *this << objName(x);
}
