// guitabbedpage.cpp

#include "guitabbedpage.h"

guiTabbedPage::guiTabbedPage(class QWidget *parent,
		 class ParamTree *ptree,
		 QString id,
		 class guiRoot *master,
		 class QRect const &geom):
  guiPage(parent, ptree, id, master, geom) {
}

void guiTabbedPage::setup(QDomElement doc) {
  guiPage::setup(doc);
}

guiTabbedPage::~guiTabbedPage() {
}
