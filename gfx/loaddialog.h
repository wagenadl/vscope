// loaddialog.h

#ifndef LOADDIALOG_H

#define LOADDIALOG_H

#include <QDialog>

class LoadDialog: public QDialog {
public:
  LoadDialog(QWidget *parent);
  virtual ~LoadDialog();
  static QString getLocalFilename(QString dir, QString filter, QString caption);
  static QString getAnywhereFilename(QString dir, QString filter, QString caption);
private:
  void populate(QString dir, QString filter, bool navigable);
  /* If navigable, present [Up] button and list of folders */
  void chdir(QString dir);
  QStringList getFiles(QString dir, QString filter);
  QStringList getFolders(QString dir);
private slots:
  void selectFile(QString fn);
  void selectFolder(QString fn);
  void doubleClickFile(QString fn);
  void doubleClickFolder(QString fn);
  void scrollFilesBack();
  void scrollFilesForward();
  void scrollFoldersBack();
  void scrollFoldersForward();
private:
  bool navigable;
  QString dir;
  QString filter;
  QString chosenfn;
  bool haschoice;
  Button *folderUp, *folderPrev, *folderNext;
  Button *filesPrev, *filesNext;
  Button *actCancel, *actOK;
  QWidget *scrollareaFolders, *scrollareaFiles;
};

#endif
