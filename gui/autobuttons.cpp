// autobuttons.cpp

#include "autobuttons.h"
#include "guichecklist.h"
#include <base/exception.h>
#include <base/dbg.h>
#include <xml/attribute.h>

AutoButtons::AutoButtons(guiButtonGroup *parent):
  QObject(parent), parent_(parent),
  initialGeom(parent->parent()) {
  isDyn = false;
}

void AutoButtons::setup(PageBuildGeom &geom,
		      QDomElement doc_) {
  initialGeom = geom;
  doc = doc_;
  enumerator = Enumerator::find(doc.attribute("enum"));
  if (!enumerator)
    throw Exception("AutoButtons", "No enumerator");
  isDyn = attributeTrue(doc, "dynamic");
  rebuild(&geom);
}

QStringList AutoButtons::selectIDs(QStringList inlist) {
  QStringList outlist;
  foreach (QString id, inlist) {
    // filter based on our xml doc
    outlist.append(id);
  }
  return outlist;
}

void AutoButtons::rebuild(PageBuildGeom *g_out) {
  QStringList newids = selectIDs(enumerator->getAllTags());
  Dbg() << "AutoButtons " << parent_->groupId() << ": rebuild " << newids.join(", ");
  if (newids == ids)
    return;

  guiPage *p = parent_->parent();
  if (!p)
    throw Exception("AutoButtons", "Grandparent is not a page");
  foreach (QString id, buttons.keys()) {
    parent_->remove(parent_->groupId() + ARRAYSEP + id);
    p->deleteButton(id);
  }

  buttons.clear();

  ids = newids;

  PageBuildGeom g = initialGeom;
  QDomDocument xml;
  QString hd1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone='yes'?>";
  QString hd2 = "<!DOCTYPE vscopeAuto>";
  foreach (QString id, ids) {
    QString fullid = parent_->groupId() + ARRAYSEP + id;
    // let's build an item
    xml.setContent(hd1 + "\n" + hd2 + "\n"
		   + "<button id=\""
		   + fullid
		   + "\" value=\""
		   + id
		   + "\"/>\n");
    QDomElement e = xml.documentElement();
    Dbg() << "Adding autobutton " << fullid;
    g.report();
    buttons[id] = p->addButton(g, e);
    parent_->add(fullid);
  }
  
  if (g_out)
    *g_out = g;
}
 
bool AutoButtons::isDynamic() const {
  return isDyn;
}
    
