// autoitems.h

#ifndef AUTOITEMS_H

#define AUTOITEMS_H

#include <gui/pagebuildgeom.h>
#include <gui/guipage.h>
#include <QString>
#include <QMap>
#include <QSet>
#include <xml/enumerator.h>

class AutoItems: public QObject {
  Q_OBJECT;
public:
  AutoItems(guiPage *parent);
  void setup(PageBuildGeom &geom,
	     QDomElement doc,
	     ParamTree *ptree);
  void rebuild(PageBuildGeom *g_out=0);
  QStringList selectIDs(QStringList inlist);
  void override(QStringList ids, PageBuildGeom *g_out=0);
  QStringList allIDs() const;
private:
  QStringList ids;
  QMap<QString, guiButton *> items;
  QDomElement doc;
  PageBuildGeom initialGeom;
  Enumerator const *enumerator;
  bool immune;
};

#endif
