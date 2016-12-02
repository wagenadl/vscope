// radiogroup.h

#ifndef RADIOGROUP_H

#define RADIOGROUP_H

#include <QObject>
#include <QPointer>
#include <QMap>
#include <gfx/button.h>

class RadioGroup: public QObject {
  Q_OBJECT;
public:
  RadioGroup(QObject *parent=0);
  virtual ~RadioGroup();
public slots:
  void add(Button *b);
  void remove(Button *b);
public:
  Button *selected();
private slots:
  void select();
  void deselect();
private:
  QMap< Button*, QPointer<Button> > buttons;
  QPointer<Button> selection;
private:
  static QMap<RadioGroup *, QPointer<RadioGroup> > allgroups;
};

#endif
