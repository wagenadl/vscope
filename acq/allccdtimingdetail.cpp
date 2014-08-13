// allccdtimingdetail.cpp

#include "allccdtimingdetail.h"
#include <xml/connections.h>
#include <base/exception.h>
#include <QSet>
#include <acq/ccdmaster.h>

AllCCDTimingDetail::AllCCDTimingDetail() {
  is_snap = false;
  foreach (QString id, Connections::allCams()) 
    details[id] = new CCDTimingDetail();
}

AllCCDTimingDetail::~AllCCDTimingDetail() {
  foreach (CCDTimingDetail *p, details)
    delete p;
}

void AllCCDTimingDetail::prepTrial(ParamTree const *ptree) {
  foreach (QString id, details.keys()) 
    details[id]->prepTrial(ptree, &::camTree(ptree, id));
  is_snap = false;
}

void AllCCDTimingDetail::prepSnap(ParamTree const *ptree) {
  foreach (QString id, details.keys()) 
    details[id]->prepSnap(ptree, &::camTree(ptree, id));
  is_snap = true;
}

int AllCCDTimingDetail::neededScans() const {
  int n = 0;
  foreach (CCDTimingDetail *p, details) 
    if (p->neededScans()>n)
      n = p->neededScans();
  return n;
}

CCDTimingDetail const &AllCCDTimingDetail::operator[](QString id) const {
  if (details.contains(id))
    return *details[id];
  else
    throw Exception("AllCCDTimingDetail", "No ccd known by " + id);
}
      
CCDTimingDetail const &AllCCDTimingDetail::first() const {
  foreach (CCDTimingDetail const *p, details)
    return *p;
  throw Exception("AllCCDTimingDetail", "No ccd");
  // yeah, that's ugly, but this is just a dev hack
}
