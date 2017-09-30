// base/dbg.cpp - This file is part of VScope.
// (C) Daniel Wagenaar 2008-1017.
/* VScope is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   VScope is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with VScope.  If not, see <http://www.gnu.org/licenses/>.
*/

// dbg.cpp

#include <base/dbg.h>
#include <base/object.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <QTime>
#include <base/exception.h>
#include <QMessageBox>

DbgBase::DbgBase(QString *string): QDebug(string), string(string) {
}

DbgBase::~DbgBase() {
  qDebug() << string->toUtf8().data();
}

Dbg::Dbg(QObject const *x): DbgBase(new QString) {
  static QTime t0(QTime::currentTime());
  *this << QString("[%1]").arg(t0.msecsTo(QTime::currentTime()) / 1e3).toUtf8().data();
  if (x) 
    *this << objName(x) << ": ";
}

Dbg::~Dbg() throw () {
  if (!dbgfile)
    dbgfile = new DbgFile();
  *dbgfile << *string;
}

// ----------------------------------------------------------------------
void warn(char const *fmt, ...) throw() {
  va_list ap;
  va_start(ap,fmt);
  char txt[10240];
  vsnprintf(txt,10240,fmt,ap);
  Warning(0) << txt;
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
  //  printf("DbgFile(%p) constructor\n",this);
  dir = "";
  ts=0;
}

DbgFile::~DbgFile() {
  //  printf("DbgFile(%p) destructor\n",this);
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
  return *this;
  if (!ts) {
    if (!dir.isEmpty()) {
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
        foreach (QString s, backlog) 
  	  *ts << s;
        backlog.clear();
      }
    }
  }
  if (ts) {
    *ts << str;
    ts->flush();
  } else {
    backlog.append(str);
  }
  return *this;
}


//////////////////////////////////////////////////////////////////////
Warning::Warning(bool gw): Dbg(), guiwarn(gw) {
  *this << "WARNING: ";
  t0 = *string;
}

bool Warning::guiwarn_enabled = false;

void Warning::enableGUIWarnings() {
  guiwarn_enabled = true;
}

void Warning::disableGUIWarnings() {
  guiwarn_enabled = false;
}

Warning::~Warning() {
  if (guiwarn && guiwarn_enabled) {
    QString s = string->mid(t0.size());
    s.replace("\"", "");
    s.replace("\\n", "\n");
    QMessageBox::warning(0, "VScope Warning", s);
  }
}
