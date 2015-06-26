// vprojector.cpp

#include "vprojector.h"
#include <xml/paramtree.h>
#include <base/dbg.h>

VProjector::VProjector(QObject *parent): QObject(parent) {
  Dbg() << "vprojector";
  proc = 0;
}

VProjector::~VProjector() {
  Dbg() << "~vprojector";
}

bool VProjector::activate() {
  Dbg() << "vprojector::activate";
  return false;
}

void VProjector::prepare(class ParamTree const *) {
  Dbg() << "vprojector::setup";
}

void VProjector::start() {
  Dbg() << "vprojector::start";
}

