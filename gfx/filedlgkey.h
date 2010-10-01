// filedlgkey.h

#ifndef FILEDLGKEY_H

#define FILEDLGKEY_H

#include <QFrame>

class FileDlgKey: public QFrame {
  /*:C FileDlgKey
   *:D A version of FileDialog with a virtual keyboard
   */
  Q_OBJECT;
public:
  FileDlgKey(QWidget *parent, bool expts=false);
  /*:F constructor
   *:D If expts=true, we create an ExptFileDialog.
   */
  virtual ~FileDlgKey();
  void mustExist(bool);
  void requireConfirm(bool);
  QString getPath() const;
public slots:
  void goDir(QString path);
  void setExtn(QString extn);
  void setLabel(QString label);
signals:
  void changedDir(QString path);
  void chosenFile(QString fn);
  void canceled();
protected:
  virtual void resizeEvent(class QResizeEvent *);
private:
  class QLabel *hdr;
  class FileDialog *fd;
  class VirtKeyboard *kb;
};

#endif
