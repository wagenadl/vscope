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