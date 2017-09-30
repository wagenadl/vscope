// toplevel/exptlog.h - This file is part of VScope.
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

// exptlog.h

#ifndef EXPTLOG_H

#define EXPTLOG_H

#include <QObject>
#include <QStringList>
#include <QMap>

class ExptLog: public QObject {
  Q_OBJECT;
public:
  ExptLog(QObject *parent=0);
  virtual ~ExptLog();
public slots:
  void prepareUserNote();
  void addNote(QString s, bool nodate=false);
  void newExptName();
  void markTrial();
  void markSnap();
  void markContEphys();
  void markContEphysEnds();
  void markLoadSettings(QString s);
  void changeSetting(QString label, QString value);
  void changeROI();
  void activate();
private slots:
  void acceptUserNote(QString);
  void cancelUserNote();
  void acceptDoubleClick(QString but, QString txt);
public:
  static QString trialID();
private:
  class TextEntry *noteEditor;
  QStringList backlog;
  bool suppressFurtherROI;
  QString fileName;
  class QFile *outputFile;
  class QTextStream *outputStream;
  QString olddate, oldtime;
  QMap<QString, QString> settingsBacklog;
private:
  void openFile();
  void writeBacklog();
  void writeSettingsBacklog();
  void makeConnections();
};

#endif
