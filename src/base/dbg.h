// base/dbg.h - This file is part of VScope.
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
  Warning(bool guiwarn=true);
  virtual ~Warning() throw();
  static void enableGUIWarnings();
  static void disableGUIWarnings();
private:
  QString t0;
  bool guiwarn;
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
