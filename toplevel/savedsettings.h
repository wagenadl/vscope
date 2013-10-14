// savedsettings.h

#ifndef SAVEDSETTINGS_H

#define SAVEDSETTINGS_H

#include <gfx/filechooser.h>

class SavedSettings: public FileChooser {
  Q_OBJECT;
public:
  SavedSettings(QWidget *parent);
  virtual ~SavedSettings();
public slots:
  void loadSettings(QString fn);
  void saveSettings(QString fn);
  void prepareSave();
  void deleteCurrent();
  void anythingChanged();
protected:
  virtual void showEvent(class QShowEvent *);
private:
  void visualdeselect();
  void visualselect(QString fn);
signals:
  void selected(QString fn);
private:
  class FileDlgKey *savedlg;
  class QFrame *saveframe;
  QString lastsel;
};

#endif
