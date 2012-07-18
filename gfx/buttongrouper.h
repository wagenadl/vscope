// buttongrouper.h

#ifndef BUTTONGROUPER_H

#define BUTTONGROUPER_H

 
#include <QMap>
#include <QSet>

class ButtonGrouper {
public:
  static void add(Button *child, QObject *parent); // may be used to reparent
  static void remove(Button *); // throws exc. if not found
  static void select(Button *); // throws exc. if not found
  static void deselect(Button *); // throws exc. if not found
  static Button *selectedSibling(Button *); // may return 0 or self
private:
  static QMap<Button *, QObject *> button2parent;
  static QMap<QObject *,QSet<Button *> > parent2buttons;
  static QMap<QObject *, Button *> parent2selected;
};

#endif
