// autoitems.cpp

#include "autoitems.h"
#include "guichecklist.h"
#include <base/exception.h>
#include <base/dbg.h>
#include <xml/attribute.h>

AutoItems::AutoItems(guiPage *parent):
  QObject(parent), initialGeom(parent) {
  isDyn = false;
}

void AutoItems::setup(PageBuildGeom &geom,
		      QDomElement doc_) {
  initialGeom = geom;
  doc = doc_;
  enumerator = Enumerator::find(doc.attribute("enum"));
  if (!enumerator)
    throw Exception("AutoItems", "No enumerator");
  isDyn = attributeTrue(doc, "dynamic");
  rebuild(&geom);
}

QStringList AutoItems::selectIDs(QStringList inlist) {
  QStringList outlist;
  foreach (QString id, inlist) {
    // filter based on our xml doc
    outlist.append(id);
  }
  return outlist;
}

void AutoItems::rebuild(PageBuildGeom *g_out) {
  QStringList newids = selectIDs(enumerator->getAllTags());
  if (newids == ids)
    return;
  
  guiPage *p = dynamic_cast<guiPage *>(parent());
  if (!p)
    throw Exception("AutoItems", "Parent is not a page");
  foreach (QString id, items.keys())
    p->deleteButton(id);

  items.clear();

  ids = newids;

  PageBuildGeom g = initialGeom;
  QDomDocument xml;
  QString hd1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone='yes'?>";
  QString hd2 = "<!DOCTYPE vscopeAuto>";
  foreach (QString id, ids) {
    // let's build an item
    xml.setContent(hd1 + "\n" + hd2 + "\n"
		   + "<item value=\""
		   + id
		   + "\"/>\n");
    QDomElement e = xml.documentElement();
    items[id] = p->addItem(g, e);
  }
  
  if (g_out)
    *g_out = g;
}
 
bool AutoItems::isDynamic() const {
  return isDyn;
}
    
