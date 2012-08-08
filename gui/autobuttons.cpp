// autobuttons.cpp

#include "autobuttons.h"
#include "guichecklist.h"
#include <base/exception.h>
#include <base/dbg.h>
#include <xml/attribute.h>

AutoButtons::AutoButtons(guiPage *page, guiRadioGroup *group):
  QObject(page), page(page), group(group),
  initialGeom(page) {
  if (!page)
    throw Exception("AutoButtons", "Missing a page");
  isDyn = false;
}

void AutoButtons::setup(PageBuildGeom &geom,
		      QDomElement doc_) {
  initialGeom = geom;
  doc = doc_;
  initialGeom.go(doc);
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
  QStringList newids = selectIDs(enumerator->getNonnegativeTags());
  if (newids == ids)
    return;

  foreach (QString id, buttons.keys()) 
    page->deleteButton(id);

  buttons.clear();

  ids = newids;

  PageBuildGeom g = initialGeom;
  QDomDocument xml;
  QString hd1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone='yes'?>";
  QString hd2 = "<!DOCTYPE vscopeAuto>";
  QString pfx = doc.hasAttribute("id")
    ? doc.attribute("id")
    : group
      ? group->id()
      : "";
  foreach (QString id, ids) {
    QString fullid = pfx + ARRAYSEP + id;
    // let's build an item
    xml.setContent(hd1 + "\n" + hd2 + "\n"
		   + "<button id=\""
		   + fullid
		   + "\" value=\""
		   + id
		   + "\"/>\n");
    QDomElement e = xml.documentElement();
    Dbg() << "Adding autobutton " << fullid;
    buttons[id] = page->addButton(g, e);
    g.up(); g.right();
    if (group)
      group->add(buttons[id]);
  }
  
  if (g_out)
    *g_out = g;
}
 
bool AutoButtons::isDynamic() const {
  return isDyn;
}
    
QList<QString> AutoButtons::childIDs() const {
  return buttons.keys();
}
