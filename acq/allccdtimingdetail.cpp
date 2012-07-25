// allccdtimingdetail.cpp

#include "allccdtimingdetail.h"
#include <xml/connections.h>

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
    details[id]->prepTrial(ptree, ptree->findp("acqCCD/camera:"+id));
  is_snap = false;
}

void AllCCDTimingDetail::prepSnap(ParamTree const *ptree) {
  foreach (QString id, details.keys()) 
    details[id]->prepSnap(ptree, ptree->findp("acqCCD/camera:"+id));
  is_snap = true;
}

int AllCCDTimingDetail::neededScans() const {
  int n = 0;
  foreach (CCDTimingDetail *p, details) 
    if (p->neededScans()>n)
      n = p->neededScans();
  return n;
}

CCDTimingDetail const *AllCCDTimingDetail::operator[](QString id) const {
  if (details.contains(id))
    return details[id];
  else
    return 0;
}
      
