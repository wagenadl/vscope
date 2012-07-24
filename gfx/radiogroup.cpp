// radiogroup.cpp

#include "radiogroup.h"

#include <base/dbg.h>

QMap<RadioGroup *, QPointer<RadioGroup> > RadioGroup::allgroups;

RadioGroup::RadioGroup(QObject *parent): QObject(parent) {
  allgroups[this] = QPointer<RadioGroup>(this);
}

RadioGroup::~RadioGroup() {
  allgroups.remove(this);
}

void RadioGroup::add(Button *b) {
  // ownership is exclusive, so remove from other groups
  foreach (QPointer<RadioGroup> const &rg, allgroups)
    if (rg!=this)
      if (!rg.isNull())
	rg->remove(b);

  buttons[b] = QPointer<Button>(b);

  b->makeRadio();
  b->setVisualType(VT_Grouped);

  connect(b, SIGNAL(selected(QString, QString)),
	  this, SLOT(select()));
  connect(b, SIGNAL(deselected(QString, QString)),
	  this, SLOT(deselect()));

}

void RadioGroup::remove(Button *b) {
  if (selection==b)
    selection=0;
  Button *p = buttons[b]; // reference through guard
  buttons.remove(b);
  if (p) {
    disconnect(p, SIGNAL(selected(QString, QString)),
	       this, SLOT(select()));
    disconnect(p, SIGNAL(deselected(QString, QString)),
	       this, SLOT(deselect()));
  }
}

void RadioGroup::select() {
  QObject *src = sender();
  Dbg() << "RadioGroup[" << this << "]: select " << src << " was " << selection;
  if (selection!=src)
    if (!selection.isNull())
      selection->setSelected(false);
  selection = dynamic_cast<Button *>(src);
  Dbg() << " -> selection=" << selection;
}

void RadioGroup::deselect() {
  QObject *src = sender();
  Dbg() << "RadioGroup[" << this << "]: deselect " << src << " was " << selection;
  if (selection==src)
    selection = 0;
}
