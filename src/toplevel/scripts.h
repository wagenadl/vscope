// toplevel/scripts.h - This file is part of VScope.
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
