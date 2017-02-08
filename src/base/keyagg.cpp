// keyagg.cpp

#include "keyagg.h"
#include <base/dbg.h>

KeyAgg::KeyAgg(QObject *parent): KeyAccess(parent) {
}

KeyAgg::~KeyAgg() {
}

void KeyAgg::add(KeyAccess *src) {
  sources.insert(src);
  connect(src, SIGNAL(newData()), this, SLOT(updateData()));
}

void KeyAgg::remove(KeyAccess *src) {
  if (sources.contains(src)) {
    disconnect(src, SIGNAL(newData()), this, SLOT(updateData()));
    sources.remove(src);
  }
}

void KeyAgg::sourceDying(QObject *o) {
  remove((KeyAccess *)o);
}

void KeyAgg::updateData() {
  //  Dbg(this) << "KeyAgg: updateData";
  emitUnlessCheckedOut();
}

void KeyAgg::emitUnlessCheckedOut() {
  //  Dbg(this) << "KeyAgg: emitUnlessCheckedOut";
  foreach (KeyAccess *src, sources)
    if (src->anyCheckedOut())
      return;
  //  Dbg(this) << "  No sources checkedout. any of mine? " << anyCheckedOut();
  // dumpObjectInfo();
  KeyAccess::emitUnlessCheckedOut();
}
