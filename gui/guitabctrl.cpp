// guitabctrl.cpp

#include "guitabctrl.h"
#include "guipage.h"
#include "pagebuildgeom.h"
#include "autobuttons.h"
#include <base/exception.h>

guiTabCtrl::guiTabCtrl(guiPage *parent):
  guiRadioGroup(parent) {
}

guiTabCtrl::~guiTabCtrl() {
}

void guiTabCtrl::build(PageBuildGeom &g0, QDomElement doc) {
  PageBuildGeom g(g0, doc);
  guiRadioGroup::build(g, doc, true);
}

