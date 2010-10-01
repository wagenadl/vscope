// exptlog.h

#ifndef EXPTLOG_H

#define EXPTLOG_H

#include <QObject>
#include <QStringList>

class ExptLog: public QObject {
  Q_OBJECT;
public:
  ExptLog(QObject *parent=0);
  virtual ~ExptLog();
public slots:
  void prepareUserNote();
  void addNote(QString s, bool nodate=false);
  void newExptName();
  void markTrial(bool snap);
  void markContEphys(bool start_not_stop);
  void markLoadSettings(QString s);
  void changeSetting(QString label);
  void changeROI();
  void activate();
private slots:
  void acceptUserNote(QString);
  void cancelUserNote();
private:
  class TextEntry *noteEditor;
  QStringList backLog;
  bool suppressFurtherROI;
  bool suppressFurtherSetting;;
  QString fileName;
  class QFile *outputFile;
  class QTextStream *outputStream;
  QString olddate, oldhr, oldmin, oldsec;
};

#endif
