// buttongrouper.cpp

#include "button.h"
#include "buttongrouper.h"

#include <base/dbg.h>
#include <base/exception.h>

QMap<Button *, QObject *> ButtonGrouper::button2parent;
QMap<QObject *,QSet<Button *> > ButtonGrouper::parent2buttons;
QMap<QObject *, Button *> ButtonGrouper::parent2selected;


void ButtonGrouper::add(Button *child, QObject *parent) {
  //  dbg("ButtonGrouper::add(%p,%p)",child,parent);
  if (button2parent.contains(child)) {
    // we already have this one, let's remove it first
    remove(child);
  }
  button2parent[child] = parent;
  parent2buttons[parent].insert(child);
}

void ButtonGrouper::remove(Button *child) {
  //  dbg("ButtonGrouper::remove(%p)",child);
  if (button2parent.contains(child)) {
    QObject *parent = button2parent.take(child);
    parent2buttons[parent].remove(child);
    if (parent2selected[parent]==child)
      parent2selected[parent]=0;
    if (parent2buttons[parent].isEmpty()) {
      parent2buttons.remove(parent);
      parent2selected.remove(parent);
    }
  } else {
    dbg("ButtonGrouper::remove(%p)",child);
    throw Exception("ButtonGrouper","Unknown button","remove");
  }
}


void ButtonGrouper::select(Button *child) {
  if (button2parent.contains(child)) {
    QObject *parent = button2parent[child];
    parent2selected[parent] = child;
  } else {
    dbg("ButtonGrouper::select(%p)",child);
    throw Exception("Button","Unknown button","select");
  }
}

void ButtonGrouper::deselect(Button *child) {
  if (button2parent.contains(child)) {
    QObject *parent = button2parent[child];
    if (parent2selected[parent]==child)
      parent2selected[parent] = 0;
  } else {
    dbg("ButtonGrouper::deselect(%p)",child);
    throw Exception("Button","Unknown button","deselect");
  }
}

Button *ButtonGrouper::selectedSibling(Button *child) {
  if (button2parent.contains(child)) {
    QObject *parent = button2parent[child];
    return parent2selected[parent];
  } else {
    dbg("ButtonGrouper::selectedSibling(%p)",child);
    throw Exception("Button","Unknown button","selectedSibling");
  }
}


