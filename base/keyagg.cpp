// keyagg.cpp

#include "keyagg.h"

KeyAgg::KeyAgg(QObject *parent): KeyAccess(parent) {
}

KeyAgg::~KeyAgg() {
}

void KeyAgg::add(KeyAccess *src) {
  sources.insert(src);
  connect(src, SIGNAL(newData()), this, SIGNAL(updateData()));
}

void KeyAgg::remove(KeyAccess *src) {
  if (sources.contains(src)) {
    disconnect(src, SIGNAL(newData()), this, SIGNAL(updateData()));
    sources.remove(src);
  }
}

void KeyAgg::sourceDying(QObject *o) {
  remove((KeyAccess *)o);
}

void KeyAgg::updateData() {
  emitUnlessCheckedOut();
}

void KeyAgg::emitUnlessCheckedOut() {
  foreach (KeyAccess *src, sources)
    if (src->anyCheckedOut())
      return;
  KeyAccess::emitUnlessCheckedOut();
}
