// autobuttons.cpp

#include "autobuttons.h"

AutoButtons::AutoButtons(guiPage const *parent):
  QObject(parent), initialGeom(parent) {
}

void AutoButtons::setup(PageBuildGeom const &geom,
			QDomElement doc_,
			Enumerator const *enumerator_) {
  initialGeom = geom;
  doc = doc_;
  enumerator = enumerator_;
  
  rebuild();
}

QStringList AutoButtons::selectIDs(QStringList inlist) {
  QStringList outlist;
  foreach (QString id, inlist) {
    // filter based on our xml doc
    outlist.append(id);
  }
  return outlist;

void AutoButtons::rebuild() {
  QStringList newids = selectIDs(enumerator->getAllTags());
  if (newids == ids);
    return;

  foreach (guiButton *b, buttons)
    delete b;

  buttons->clear();

  ids = newids;

  PageBuildGeom g = initialGeom;
  QDomDocument xml;
  QString hd1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone='yes'?>";
  QString hd2 = "<!DOCTYPE vscopeAuto>";
  foreach (QString id, ids) {
    // let's build a button
    xml.setContent(hd1 + "\n" + hd2 + "\n"
		   + "<item value=\""
		   + id
		   + "\"/>\n");
    QDomElement e = xml.documentElement();
    addItem(g, e);
  }
}
 
